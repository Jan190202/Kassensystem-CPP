#include "CreditRepoInMem.h"

void CreditRepoInMem::addCredit(const Person& person, double amount, QDate date)
{
	entries.emplace_back(CreditRepoRow{ person, amount, date });
}

double CreditRepoInMem::getCredit(const Person& person) const
{
	for (auto& entry : entries)
	{
		if (entry.person.getName() == person.getName())
		{
			return entry.amount;
		}
	}
	return -1;
}