#include "ConsumptionRepoInMem.h"
#include <vector>

#include <QDebug>

int64_t ConsumptionRepoInMem::addEntry(const entry::Consumption& entry)
{
	data.push_back(entry);

	qInfo() << "entry::Consumption added! Entries: ";
	for (auto& entry : data)
	{
		qInfo()
			<< " consumptionEntryID:" << entry.consumptionEntryID
			<< " debtEntryID:" << entry.debtEntryID
			<< " nBeer04:" << entry.nBeer04
			<< " nBeer05:" << entry.nBeer05
			<< " nSoftdrinks:" << entry.nSoftdrinks
			<< " nWater:" << entry.nWater
			<< " otherExpense:" << entry.otherExpense;
	}

	return 0;
}

std::vector<entry::Consumption> ConsumptionRepoInMem::getEntries(int64_t personID) const
{
	std::vector<entry::Consumption> filteredData = {};

	for (auto& entry : data)
	{
		if (true) // implementation in SQL reads from different table (debtRepository)
		{
			filteredData.push_back(entry);
		}
	}

	return filteredData;
}