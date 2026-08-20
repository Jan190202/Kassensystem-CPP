#pragma once

#include "entities/ConsumptionEntry.h"
#include "repoInterface/ConsumptionRepository.h"
#include <vector>

class ConsumptionService
{
public:
	ConsumptionService(ConsumptionRepository* consumptionRepo);
	void addConsumption(const ConsumptionEntry& consumptionEntry);
	
	double getTotal(int personID) const;
	double calculateTotal(const ConsumptionEntry&) const;
	std::vector<ConsumptionEntry> getEntries(int personID) const;
private:
	ConsumptionRepository* consumptionRepo;
};






