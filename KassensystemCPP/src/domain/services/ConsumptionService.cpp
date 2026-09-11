#include "ConsumptionService.h"
#include <QDebug>
#include <regex>

ConsumptionService::ConsumptionService(const RepositoryBundle& repoBundle, const PriceList& priceList)
	: consumptionRepo(repoBundle.consumptionRepo), debtRepo(repoBundle.debtRepo), personRepo(repoBundle.personRepo), priceList(priceList) {}

void ConsumptionService::addConsumption(const request::Consumption& request)
{
	double amount = calculateDebt(request);
	
	// find or create Person entry
	int64_t personEntryID{};
	if (std::holds_alternative<std::string>(request.personInput)) // new name, no avaliable ID
	{		
		std::string nameRequest = std::get<std::string>(request.personInput);

		if (nameRequest == "") return; // skip entries without name

		auto result = isValidNameFormat(nameRequest);

		if (result.has_value())
		{
			PersonStringSpecifiers spec = result.value();
			personEntryID = personRepo->addPersonEntry(
				entry::Person{
					.firstName = spec.firstName,
					.lastName = spec.lastName,
					.nickName = spec.nickName,
					.info = spec.info,
					.personEntryID = 0
				});
		}
		else
		{
			using enum validityError::Name;
			std::string debugString;

			switch (result.error())
			{
			case FirstOrLastNameMissing: debugString = "FirstOrLastNameMissing"; break;
			case UnbalancedParentheses: debugString = "UnbalancedParentheses"; break;
			case InvalidNicknameFormat: debugString = "InvalidNicknameFormat"; break;
			case TooManyComponents: debugString = "TooManyComponents"; break;
			}
			
			qDebug() << "Error Code " << static_cast<int>(result.error()) << ": " << debugString;
		}	
	}
	else
	{
		personEntryID = std::get<int64_t>(request.personInput);
	}

	// add debt and consumption entry
	entry::Debt dEntry{ .debtEntryID = 0, .personEntryID = personEntryID, .date = request.date, .amount = amount};
	int64_t dEntryID = debtRepo->addDebtEntry(dEntry);

	entry::Consumption cEntry{ .consumptionEntryID = 0, .debtEntryID = dEntryID, .nBeer05 = request.nBeer05 , .nBeer04 = request.nBeer04, .nSoftdrinks = request.nSoftdrinks, .nWater = request.nWater };
	int64_t cEntryID = consumptionRepo->addConsumptionEntry(cEntry);
}

double ConsumptionService::calculateDebt(const request::Consumption& request) const
{
	return
		priceList.beer04			* request.nBeer04 +
		priceList.beer05			* request.nBeer05 +
		priceList.water				* request.nWater +
		priceList.softdrink			* request.nSoftdrinks +
		request.otherExpense;
}

std::expected< PersonStringSpecifiers, validityError::Name > ConsumptionService::isValidNameFormat(const std::string& nameRequest) const
{
	using enum validityError::Name;
	
	PersonStringSpecifiers result;

	std::string trimmedRequest = std::regex_replace(nameRequest, std::regex(R"(^\s+|\s+$)"), "");
	
	// separate mandatory name part from the optional parentheses part
	// matches anything up to an optional "(" followed by optional content inside ()
	std::regex basePattern(R"(^([^\(]+)(?:\((.*)\))?$)"); // R"(...)" --> raw string literal
		// ^([^\(]+)(?:\((.*)\))?\s*$
			// ^([^\(]+)		--> first capture group: anything in the beginning thats not a "("
			// (?:\((.*)\))?	--> second capture group: parenthesis content (.*) inside non-capturing group of at most one whole parenthesis (?:...)
	std::smatch baseMatches;

	if (!std::regex_match(trimmedRequest, baseMatches, basePattern))
	{
		size_t openCount = std::count(trimmedRequest.begin(), trimmedRequest.end(), '(');
		size_t closeCount = std::count(trimmedRequest.begin(), trimmedRequest.end(), ')');

		if (openCount != closeCount)
		{
			return std::unexpected(UnbalancedParentheses);
		}
		return std::unexpected(TooManyComponents);
	}

	std::string namePart = baseMatches[1].str();
	std::string addPart = baseMatches[2].str();
	bool hasParentheses = baseMatches[2].matched;

	// detect closing parenthesis without an opening one (lands inside namePart)
	if (!hasParentheses && trimmedRequest.find(')') != std::string::npos)
	{
		return std::unexpected(UnbalancedParentheses);
	}

	// parse first and last name
	std::regex namePattern(R"(^\s*(\w+)\s+(\w+)\s*$)");
	std::smatch nameMatches;
	if (!std::regex_match(namePart, nameMatches, namePattern)) // namePattern doesn't have exactly two words
	{
		std::regex oneWordCheck(R"(^\s*\w+\s*$)");
		if (std::regex_match(namePart, oneWordCheck))  // namePattern has exactly one word
		{
			return std::unexpected(FirstOrLastNameMissing);
		}
		return std::unexpected(TooManyComponents); // namePattern has more than two words
	}

	result.firstName = nameMatches[1].str(); // first capture group (\w+)
	result.lastName = nameMatches[2].str(); // second capture group (\w+)

	// parse parenthesis part (if existing)
	if (hasParentheses) 
	{
		// trim whitespaces at parenthesis
		addPart = std::regex_replace(addPart, std::regex(R"(^\s+|\s+$)"), "");

		if (addPart.empty()) 
		{
			result.nickName = "";
			result.info = "";
		}

		// case ("X", Y)
		// quotes are mandatory for nickname if a comma separator is found
		std::regex bothPattern(R"regex(^"([^\"]+)"\s*,\s*(.+)$)regex"); // use custom delimiter "regex" for working string literal compilation string contains standard delimiter "( ... )"
			// case ("X")
			std::regex nickOnlyPattern(R"regex(^"([^\"]+)"$)regex"); // use custom delimiter "regex" for working string literal compilation string contains standard delimiter "( ... )"
			std::smatch innerMatches;

		if (std::regex_match(addPart, innerMatches, bothPattern)) // ("X",Y)
		{
			result.nickName = innerMatches[1].str();
			result.info = innerMatches[2].str();
		}
		else if (std::regex_match(addPart, innerMatches, nickOnlyPattern)) // ("X")
		{
			result.nickName = innerMatches[1].str();
			result.info = "";
		}
		else // neither ("X",Y) nor ("X") --> check (X, Y)
		{
			if (addPart.find('"') == std::string::npos) // no quotes at all --> (Y)
			{
				result.nickName = "";
				result.info = addPart;
			}
			else // broken quotes, e.g. ("X, Y)
			{
				return std::unexpected(InvalidNicknameFormat);
			}
		}
	}
	return result;
}

std::expected<void, validityError::Code> ConsumptionService::isRequestValid(const request::Consumption& request) const
{
	using namespace validityError;

	// check amounts < 0
	if (request.nBeer04 < 0 || request.nBeer05 < 0 || request.nSoftdrinks < 0 || request.nWater < 0 || request.otherExpense + 1e-9 < 0) 
		return std::unexpected(Consumption::SomeEntriesSmallerThanZero);

	// check zero-entry;
	if (calculateDebt(request) < 1e-9)
		return std::unexpected(Consumption::EmptyConsumptionEntries);

	// check date <= today
	if (request.date > QDate::currentDate()) 
		return std::unexpected(Date::DateLaterThanCurrentDate);

	// check name format
	if (std::holds_alternative<std::string>(request.personInput))
	{
		const auto nameValidity = isValidNameFormat(std::get<std::string>(request.personInput));
		if (!nameValidity.has_value())
			return std::unexpected(nameValidity.error());
	}

	return {};
}