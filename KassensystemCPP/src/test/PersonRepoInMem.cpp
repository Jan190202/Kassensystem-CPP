#include "PersonRepoInMem.h"

#include "entities/Person.h"

#include <string>
#include <vector>
#include <random>

Person PersonRepoInMem::addPerson(const std::string& name)
{
	int id = getUniqueID();
	Person person{ name, id };
	entries.push_back(person);
	return person;
}

int PersonRepoInMem::getUniqueID()
{
	while (true)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distrib(1, 100);
		int id = distrib(gen);

		if (isUnique(id)) return id;
	}
	
}

bool PersonRepoInMem::isUnique(int id)
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