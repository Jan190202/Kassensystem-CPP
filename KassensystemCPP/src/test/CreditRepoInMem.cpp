#include "CreditRepoInMem.h"
#include "IDGenerator.h"
#include <QDebug>

int64_t CreditRepoInMem::addCreditEntry(entry::Credit entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).creditEntryID;
	entry.creditEntryID = idgen::getID(usedIDs);

	entries.emplace_back(entry);

	qInfo() << entry;

	return entry.creditEntryID;
}

double CreditRepoInMem::getPersonsCredit(int64_t personID) const
{
	double credit{};
	
	for (auto& entry : entries)
	{
		if (entry.personID == personID)
		{
			credit += entry.amount;
		}
	}
	return credit;
}

double CreditRepoInMem::getTotalDepositedCredit() const
{
	double credit{};

	for (auto& entry : entries)
	{
		credit += entry.amount;
	}

	return credit;
}