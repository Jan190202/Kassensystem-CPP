#include "BalanceRepoInMem.h"
#include <vector>

#include <QDebug>

int64_t BalanceRepoInMem::addEntry(const entry::Balance& entry)
{
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
	return 0;
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