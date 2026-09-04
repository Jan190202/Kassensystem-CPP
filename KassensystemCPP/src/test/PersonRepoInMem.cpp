#include "PersonRepoInMem.h"
#include "IDGenerator.h"

#include <string>
#include <vector>

#include <QDebug>


std::expected<Person,std::string> PersonRepoInMem::findEntry(int64_t personID)
{
	for (auto& entry : entries)
	{
		if (entry.getID() == personID)
		{
			return entry;
		}
	}

	return std::unexpected("Entry not found!");
}

Person PersonRepoInMem::addEntry(const std::string& firstName, const std::string& lastName, const std::string& nickName, const std::string& info)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).getID();
	int64_t id = idgen::getID(usedIDs);

	Person person{ firstName, lastName, id, nickName, info };
	entries.push_back(person);

	qInfo() << "PersonEntry added! Entries: ";
	for (auto& entry : entries)
	{
		qInfo()
			<< " ID:" << entry.getID()
			<< " name:" << entry.getFullSpecifier();
	}

	return person;
}

std::vector<Person> PersonRepoInMem::getAll() const
{
	return entries;
}
