#pragma once

#include <string>
#include "entities/Person.h"

class PersonRepository
{
public:
	PersonRepository() = default;
	virtual ~PersonRepository() = default;
	virtual Person addPerson(std::string name) = 0;
};