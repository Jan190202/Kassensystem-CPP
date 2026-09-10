#pragma once
#include "domain/model/Entities.h"
#include <vector>

class ConsumptionRepository
{
public:
	ConsumptionRepository() = default;
	virtual ~ConsumptionRepository() = default;
	virtual int64_t addConsumptionEntry(entry::Consumption entry) = 0; // add consumption entry to database, return associated ID

	virtual std::vector<entry::Consumption> getConsumptionEntries(int64_t personID) const = 0; // return all consumption entries associated with person
};