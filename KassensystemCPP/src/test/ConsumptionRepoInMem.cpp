#include "ConsumptionRepoInMem.h"
#include <vector>

int64_t ConsumptionRepoInMem::addEntry(const ConsumptionEntry& entry)
{
	data.push_back(entry);
	return 0;
}

std::vector<ConsumptionEntry> ConsumptionRepoInMem::getEntries(int64_t personID) const
{
	std::vector<ConsumptionEntry> filteredData = {};

	for (auto& entry : data)
	{
		if (true) // implementation in SQL reads from different table (debtRepository)
		{
			filteredData.push_back(entry);
		}
	}

	return filteredData;
}