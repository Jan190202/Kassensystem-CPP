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

double CreditRepoInMem::getPersonsCredit(int64_t personEntryID) const
{
	double credit{};
	
	for (auto& entry : entries)
	{
		if (entry.personEntryID == personEntryID)
		{
			credit += entry.amount;
		}
	}
	return credit;
}

double CreditRepoInMem::getTotalDepositedCredit(const QDate& minDate) const
{
	double credit{};

	for (auto& entry : entries)
	{
		if (entry.date < minDate) continue;
		credit += entry.amount;
	}

	return credit;
}