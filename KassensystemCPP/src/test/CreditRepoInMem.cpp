#include "CreditRepoInMem.h"
#include "IDGenerator.h"

#include <QDebug>

int64_t CreditRepoInMem::addEntry(entry::Credit entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).creditEntryID;
	entry.creditEntryID = idgen::getID(usedIDs);

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

	return entry.creditEntryID;
}

double CreditRepoInMem::getCredit(int64_t personID) const
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

void CreditRepoInMem::resetCredit(int64_t personID)
{
	// usage: std::erase_if(container, predicate) where predicate can be a lambda taking an entry and returns true/false (remove/keep)
	std::erase_if(entries, [=](const auto& entry)
		{
			return entry.personID == personID;
		});
}