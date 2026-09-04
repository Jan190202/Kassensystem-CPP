#include "BalanceRepoInMem.h"
#include "IDGenerator.h"

#include <vector>

#include <QDebug>

int64_t BalanceRepoInMem::addEntry(entry::Balance entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).balanceEntryID;
	entry.balanceEntryID = idgen::getID(usedIDs);
	
	entries.push_back(entry);

	qInfo() << "entry::Balance added! Entries: ";
	for (auto& entry : entries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " balanceEntryID:" << entry.balanceEntryID
			<< " type:" << static_cast<int>(entry.type)
			<< " comment:" << entry.comment
			<< " date:" << entry.date
			<< " description:" << entry.description
			<< " personID:" << entry.personID;
	}
	return entry.balanceEntryID;
}

double BalanceRepoInMem::getTotalEarnings() const
{
	double totalEarnings = 0;

	for (auto& entry : entries)
	{
		if (entry.type == BalanceType::Earning)
		{
			totalEarnings += entry.amount;
		}
	}

	return totalEarnings;
}

double BalanceRepoInMem::getTotalSpendings() const
{
	double totalSpendings = 0;

	for (auto& entry : entries)
	{
		if (entry.type == BalanceType::Spending)
		{
			totalSpendings += entry.amount;
		}
	}

	return totalSpendings;
}