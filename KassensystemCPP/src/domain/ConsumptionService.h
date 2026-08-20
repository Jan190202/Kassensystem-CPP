#pragma once

#include "entities/Person.h"
#include "entities/ConsumptionEntry.h"
#include "entities/ConsumptionSummary.h"
#include "repoInterface/ConsumptionRepository.h"
#include <vector>

class ConsumptionService
{
public:
	ConsumptionService(ConsumptionRepository* consumptionRepo);
	void addConsumption(const ConsumptionEntry& consumptionEntry);
	
	double getTotal(const Person& person) const;
	std::vector<ConsumptionEntry> getEntries(const Person& person) const;
	//ConsumptionSummary getSummary(const Person& person) const;
private:
	ConsumptionRepository* consumptionRepo;
};






