#pragma once

#include "entities/ConsumptionEntry.h"
#include <vector>

class ConsumptionRepository
{
public:
	ConsumptionRepository() = default;
	virtual ~ConsumptionRepository() = default;

	virtual void addEntry(const ConsumptionEntry&) = 0;
	virtual std::vector<ConsumptionEntry> getEntries(int personID) const = 0;
};