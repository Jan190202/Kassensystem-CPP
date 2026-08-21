#include "PersonRepoInMem.h"

#include <string>
#include <vector>
#include <random>

Person PersonRepoInMem::findOrCreateEntry(const std::string& name)
{
	for (auto& entry : entries)
	{
		if (entry.getName() == name)
		{
			return entry;
		}

	}
	
	// name not found -> create person
	return addEntry(name);
}

Person PersonRepoInMem::addEntry(const std::string& name)
{
	int64_t id = getUniqueID();
	Person person{ name, id };
	entries.push_back(person);
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