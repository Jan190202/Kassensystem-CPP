#pragma once

#include <string>
#include "domain/model/Entities.h"

class PersonRepository
{
public:
	PersonRepository() = default;
	virtual ~PersonRepository() = default;
	virtual Person findOrCreateEntry(const std::string& name) = 0;
private:
	virtual Person addEntry(const std::string& name) = 0;
};