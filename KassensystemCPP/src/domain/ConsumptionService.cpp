#include "ConsumptionService.h"

#include "entities/Person.h"
#include "entities/ConsumptionEntry.h"
#include "entities/ConsumptionSummary.h"
#include "repoInterface/ConsumptionRepository.h"
#include <vector>

ConsumptionService::ConsumptionService(ConsumptionRepository* consumptionRepo)
{
	this->consumptionRepo = consumptionRepo;
}

void ConsumptionService::addConsumption(const ConsumptionEntry& consumptionEntry)
{
	consumptionRepo->addEntry(consumptionEntry);
}

double ConsumptionService::getTotal(const Person& person) const
{
	return consumptionRepo->getTotal(person);
}

std::vector<ConsumptionEntry> ConsumptionService::getEntries(const Person& person) const
{
	return consumptionRepo->getEntries(person);
}

//ConsumptionSummary ConsumptionService::getSummary(const Person& person) const
//{
//    return consumptionRepo->getSummary(person);
//}