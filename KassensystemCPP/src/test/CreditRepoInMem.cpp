#include "CreditRepoInMem.h"

void CreditRepoInMem::addCredit(int personID, double amount, QDate date)
{
	entries.emplace_back(CreditRepoRow{ personID, amount, date });
}

double CreditRepoInMem::getCredit(int personID) const
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