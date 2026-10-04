#include "ConsumptionService.h"
#include <QDebug>

ConsumptionService::ConsumptionService(PersonService& personService, const RepositoryBundle& repoBundle, const PriceList& priceList, PendingChangeLog& log)
	: personService(personService), consumptionRepo(repoBundle.consumptionRepo), debtRepo(repoBundle.debtRepo), personRepo(repoBundle.personRepo), priceList(priceList), log(log) {}

void ConsumptionService::addConsumption(const request::Consumption& request)
{
	double amount = calculateDebt(request);
	
	int64_t personEntryID{};
	if (auto result = personService.findOrCreatePerson(request.personInput); !result.has_value()) return;
	else personEntryID = result.value();

	// add debt and consumption entry
	entry::Debt dEntry{ 
		.debtEntryID = 0, 
		.personEntryID = personEntryID, 
		.dateBooked = request.dateBooked, 
		.dateAdded = QDate::currentDate(), 
		.amount = amount
	};

	int64_t dEntryID = debtRepo->addDebtEntry(dEntry);

	entry::Consumption cEntry{ 
		.consumptionEntryID = 0, 
		.debtEntryID = dEntryID, 
		.nBeer05 = request.nBeer05 , 
		.nBeer04 = request.nBeer04, 
		.nSoftdrinks = request.nSoftdrinks, 
		.nWater = request.nWater, 
		.otherExpense = request.otherExpense 
	};

	int64_t cEntryID = consumptionRepo->addConsumptionEntry(cEntry);

	log.record(PendingChangeLog::ChangeType::add, dEntry, std::optional<entry::Consumption>{cEntry});
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
	if (request.dateBooked > QDate::currentDate()) 
		return std::unexpected(Date::DateLaterThanCurrentDate);

	// check name format
	if (std::holds_alternative<std::string>(request.personInput))
	{
		const auto nameValidity = personService.isValidNameFormat(std::get<std::string>(request.personInput));
		if (!nameValidity.has_value())
			return std::unexpected(nameValidity.error());
	}

	return {};
}