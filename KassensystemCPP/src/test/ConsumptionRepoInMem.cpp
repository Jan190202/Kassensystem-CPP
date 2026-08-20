#include "ConsumptionRepoInMem.h"
#include <vector>
#include "entities/Person.h"
#include "entities/ConsumptionEntry.h"

ConsumptionRepoInMem::ConsumptionRepoInMem()
{
	data = {};
}

ConsumptionRepoInMem::~ConsumptionRepoInMem() {}

void ConsumptionRepoInMem::addEntry(const ConsumptionEntry& entry)
{
	data.push_back(entry);
}

std::vector<ConsumptionEntry> ConsumptionRepoInMem::getEntries(int personID) const
{
	std::vector<ConsumptionEntry> filteredData = {};

	for (auto& entry : data)
	{
		if (entry.personID == personID)
		{
			filteredData.push_back(entry);
		}
	}

	return filteredData;
}