#include "ConsumptionRepoInMem.h"
#include <vector>
#include "entities/ConsumptionSummary.h"
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

double ConsumptionRepoInMem::getTotal(const Person& person) const
{
	double total = 0;

	for (auto& entry : data)
	{
		total += 0; // TBD
	}

	return total;
}

std::vector<ConsumptionEntry> ConsumptionRepoInMem::getEntries(const Person& person) const
{
	std::vector<ConsumptionEntry> filteredData = {};

	for (auto& entry : data)
	{
		if (entry.personID == person.getID())
		{
			filteredData.push_back(entry);
		}
	}

	return filteredData;
}

//ConsumptionSummary ConsumptionRepoInMem::getSummary(const Person& person)
//{
//	double total = 0, paid = 0, due = 0, credit = 0;
//
//	for (auto& entry : data)
//	{
//		total += entry.total;
//	}
//
//	
//	ConsumptionSummary c{ person, total, paid, due, credit, data };
//	return c;
//}