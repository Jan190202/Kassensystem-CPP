#include "ConsumptionService.h"

#include "entities/Person.h"
#include "entities/ConsumptionEntry.h"
#include "repoInterface/ConsumptionRepository.h"
#include "domain/PriceList.h"
#include <vector>

ConsumptionService::ConsumptionService(ConsumptionRepository* consumptionRepo)
{
	this->consumptionRepo = consumptionRepo;
}

void ConsumptionService::addConsumption(const ConsumptionEntry& consumptionEntry)
{
	consumptionRepo->addEntry(consumptionEntry);
}

std::vector<ConsumptionEntry> ConsumptionService::getEntries(int personID) const
{
	return consumptionRepo->getEntries(personID);
}

double ConsumptionService::getTotal(int personID) const
{
	std::vector<ConsumptionEntry> entries = getEntries(personID);

	ConsumptionEntry entrySum;

	for (auto& entry : entries)
	{
		entrySum.nBeer04 += entry.nBeer04;
		entrySum.nBeer05 += entry.nBeer05;
		entrySum.nSoftdrinks += entry.nSoftdrinks;
		entrySum.nWater += entry.nWater;
	}

	return calculateTotal(entrySum);
}

double ConsumptionService::calculateTotal(const ConsumptionEntry& entry) const
{
	return
		PriceList::beer04		* entry.nBeer04 +
		PriceList::beer05		* entry.nBeer05 +
		PriceList::water		* entry.nWater  +
		PriceList::softdrink	* entry.nSoftdrinks;
}