#include "PersonRepoInMem.h"

#include <string>
#include <vector>
#include <random>

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
	int64_t id = getUniqueID();

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

int64_t PersonRepoInMem::getUniqueID()
{
	while (true)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int64_t> distrib(1, 100);
		int64_t id = distrib(gen);

		if (isUnique(id)) return id;
	}
	
}

bool PersonRepoInMem::isUnique(int64_t id)
{
	for (auto& entry : entries)
	{
		if (entry.getID() == id)
		{
			return false;
		}
	}
	return true;
}

std::vector<Person> PersonRepoInMem::getAll() const
{
	return entries;
}
