#include "ConsumptionRepoInMem.h"
#include "IDGenerator.h"

#include <vector>

#include <QDebug>

int64_t ConsumptionRepoInMem::addEntry(entry::Consumption entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).consumptionEntryID;
	entry.consumptionEntryID = idgen::getID(usedIDs);

	entries.push_back(entry);

	qInfo() << "entry::Consumption added! Entries: ";
	for (auto& entry : entries)
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

	return entry.consumptionEntryID;
}

std::vector<entry::Consumption> ConsumptionRepoInMem::getEntries(int64_t personID) const
{
	std::vector<entry::Consumption> filteredData = {};

	for (auto& entry : entries)
	{
		if (true) // implementation in SQL reads from different table (debtRepository)
		{
			filteredData.push_back(entry);
		}
	}

	return filteredData;
}