#include "CreditRepoInMem.h"

#include <QDebug>

int64_t CreditRepoInMem::addEntry(const entry::Credit& entry)
{
	entries.emplace_back(entry);

	qInfo() << "entry::Credit added! Entries: ";
	for (auto& entry : entries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " creditEntryID:" << entry.creditEntryID
			<< " date:" << entry.date
			<< " description:" << entry.description
			<< " personID:" << entry.personID;
	}

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
	return 0;
}