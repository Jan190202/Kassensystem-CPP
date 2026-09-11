#include "PersonRepoInMem.h"
#include "IDGenerator.h"
#include <string>
#include <vector>
#include <QDebug>

std::expected<entry::Person,std::string> PersonRepoInMem::findPersonEntry(int64_t personID) const
{
	for (auto& entry : entries)
	{
		if (entry.id == personID)
		{
			return entry;
		}
	}

	return std::unexpected("Entry not found!");
}

int64_t PersonRepoInMem::addPersonEntry(entry::Person entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).id;
	int64_t id = idgen::getID(usedIDs);
	entry.id = id;
	entries.push_back(entry);

	qInfo() << entry;

	return id;
}

std::vector<entry::Person> PersonRepoInMem::getAllPersonEntries() const
{
	return entries;
}
