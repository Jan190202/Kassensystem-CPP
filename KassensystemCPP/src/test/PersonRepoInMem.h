#pragma once

#include "domain/repoInterface/PersonRepository.h"
#include "entities/Person.h"

#include <string>
#include <vector>

class PersonRepoInMem : public PersonRepository
{
public:
	PersonRepoInMem() = default;
	virtual ~PersonRepoInMem() = default;
	virtual Person addPerson(std::string name);
private:
	std::vector<Person> entries = {};
	int getUniqueID();
	bool isUnique(int id);
};