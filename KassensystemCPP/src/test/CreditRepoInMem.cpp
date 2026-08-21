#include "CreditRepoInMem.h"

int64_t CreditRepoInMem::addEntry(const CreditEntry& entry)
{
	entries.emplace_back(entry);
	return 0;
}

double CreditRepoInMem::getCredit(int64_t personID) const
{
	for (auto& entry : entries)
	{
		if (entry.personID == personID)
		{
			return entry.amount;
		}
	}
	return -1;
}