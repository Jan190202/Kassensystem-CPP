#pragma once

#include "domain/model/Entities.h"
#include <vector>

class ConsumptionRepository
{
public:
	ConsumptionRepository() = default;
	virtual ~ConsumptionRepository() = default;

	virtual int64_t addEntry(const ConsumptionEntry&) = 0;
	virtual std::vector<ConsumptionEntry> getEntries(int64_t personID) const = 0;
};