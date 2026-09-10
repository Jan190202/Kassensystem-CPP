#pragma once

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/repointerface/ConsumptionRepository.h"
#include "domain/repointerface/DebtRepository.h"
#include "domain/repointerface/PersonRepository.h"
#include <vector>
#include <expected>
#include <variant>

namespace validityError
{
	enum Name
	{
		FirstOrLastNameMissing,
		UnbalancedParentheses,
		InvalidNicknameFormat,
		TooManyComponents
	};

	enum Date
	{
		DateLaterThanCurrentDate
	};

	enum Consumption
	{
		SomeEntriesSmallerThanZero,
		EmptyConsumptionEntries
	};

	using Code = std::variant<Name, Date, Consumption>;
}

struct PersonStringSpecifiers
{
	std::string firstName, lastName, nickName, info;
};


class ConsumptionService
{
public:
	ConsumptionService(ConsumptionRepository* consumptionRepo, DebtRepository* debtRepo, PersonRepository* personRepo, const PriceList& priceList);

	std::expected<void,validityError::Code> isRequestValid(const request::Consumption& request) const;
	void addConsumption(const request::Consumption& request);
	double calculateDebt(const request::Consumption&) const;
	std::vector<entry::Consumption> getEntries(int personID) const;
private:

	std::expected< PersonStringSpecifiers, validityError::Name > isValidNameFormat(const std::string& nameRequest) const;

	ConsumptionRepository* consumptionRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
	const PriceList& priceList;
};






