#include "DebtRepoInMem.h"

#include <QDebug>

int64_t DebtRepoInMem::addEntry(const entry::Debt& entry)
{
	entries.push_back(entry);

	qInfo() << "entry::Debt added! Entries: ";
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

std::vector<entry::DebtRemaining> DebtRepoInMem::getOutstandingEntries(int64_t personID, FilterType filter) const
{
	std::vector<entry::DebtRemaining> filteredEntries = {};

	for (auto& entry : entries)
	{
		double remaining = entry.amount; // implementation later using SQL 

		if (filter == FilterType::OmitFullyPaid && remaining < 1e-9) continue;
		
		entry::DebtRemaining entryRem;
		entryRem.debtEntryID = entry.debtEntryID;
		entryRem.date = entry.date;
		entryRem.amount = entry.amount;
		entryRem.remaining = remaining; 

		filteredEntries.push_back(entryRem);
	}

	return filteredEntries;
}