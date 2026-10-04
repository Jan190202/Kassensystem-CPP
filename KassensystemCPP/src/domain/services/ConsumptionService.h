#pragma once
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "app/RepositoryBundle.h"
#include "domain/services/PersonService.h"
#include <vector>
#include <expected>
#include <variant>

namespace validityError
{
	enum class Date
	{
		DateLaterThanCurrentDate
	};

	enum class Consumption
	{
		SomeEntriesSmallerThanZero,
		EmptyConsumptionEntries
	};

	using Code = std::variant<Name, Date, Consumption>;
}

class ConsumptionService
{
public:
	ConsumptionService(PersonService& personService, const RepositoryBundle& repoBundle, const PriceList& priceList, PendingChangeLog& log);
	void addConsumption(const request::Consumption& request);

	std::expected<void,validityError::Code> isRequestValid(const request::Consumption& request) const;
	double calculateDebt(const request::Consumption& request) const;

private:
	PersonService& personService;
	ConsumptionRepository* consumptionRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
	const PriceList& priceList;
	PendingChangeLog& log;
};