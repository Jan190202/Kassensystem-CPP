#pragma once

#include "domain/model/Entities.h"
#include <vector>

class ConsumptionRepository
{
public:
	ConsumptionRepository() = default;
	virtual ~ConsumptionRepository() = default;

	virtual int64_t addEntry(entry::Consumption) = 0;
	virtual std::vector<entry::Consumption> getEntries(int64_t personID) const = 0;
};