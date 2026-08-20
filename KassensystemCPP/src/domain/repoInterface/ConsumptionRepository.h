#pragma once

#include "entities/ConsumptionEntry.h"
#include "entities/ConsumptionSummary.h"
#include <vector>

class ConsumptionRepository
{
public:
	ConsumptionRepository() = default;
	virtual ~ConsumptionRepository() = default;

	virtual void addEntry(const ConsumptionEntry&) = 0;
	
	virtual double getTotal(const Person&) const = 0;
	virtual std::vector<ConsumptionEntry> getEntries(const Person&) const = 0;
	//virtual ConsumptionSummary getSummary(const Person&) const = 0;
};