#include "DebtRepoInMem.h"

#include <QDebug>

int64_t DebtRepoInMem::addEntry(const DebtEntry& entry)
{
	entries.push_back(entry);

	qInfo() << "DebtEntry added! Entries: ";
	for (auto& entry : entries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " date:" << entry.date
			<< " debtID:" << entry.debtEntryID
			<< " personID:" << entry.personID;
	}

	return 0;
}

double DebtRepoInMem::getTotal(int64_t personID) const
{
	double amount = 0;

	for (auto& entry : entries)
	{
		amount += entry.amount;
	}

	return amount;
}

std::vector<DebtEntryRemaining> DebtRepoInMem::getOutstandingEntries(int64_t personID) const
{
	std::vector<DebtEntryRemaining> filteredEntries = {};

	for (auto& entry : entries)
	{
		if (true) // implementation later using SQL and lookup in other tables
		{
			DebtEntryRemaining entryRem;
			entryRem.entryID = entry.debtEntryID; 
			entryRem.date = entry.date; 
			entryRem.amount = entry.amount; 
			entryRem.remaining = entry.amount; // implementation later using SQL 

			filteredEntries.push_back(entryRem);
		}
	}

	return filteredEntries;
}