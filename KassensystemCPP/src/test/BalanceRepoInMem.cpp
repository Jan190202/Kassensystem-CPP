#include "BalanceRepoInMem.h"
#include <vector>

int64_t BalanceRepoInMem::addEntry(const BalanceEntry& entry)
{
	entries.push_back(entry);
	return 0;
}

double BalanceRepoInMem::getTotalEarnings() const
{
	double totalEarnings = 0;

	for (auto& entry : entries)
	{
		if (entry.type == BalanceType::earning)
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
		if (entry.type == BalanceType::spending)
		{
			totalSpendings += entry.amount;
		}
	}

	return totalSpendings;
}