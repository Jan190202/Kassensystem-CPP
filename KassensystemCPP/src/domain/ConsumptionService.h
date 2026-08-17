#pragma once

#include "entities/ConsumptionEntry.h"

class ConsumptionService
{
public:
	ConsumptionService() = default;
	void addConsumption(const ConsumptionEntry& consumptionEntry);
};