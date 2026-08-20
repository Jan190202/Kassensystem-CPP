#include "BalanceRepoInMem.h"
#include "domain/DomainTypes.h"
#include "entities/BalanceEntry.h"

#include <vector>

void BalanceRepoInMem::addBalanceEntry(const BalanceEntry& entry)
{
	entries.push_back(entry);
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