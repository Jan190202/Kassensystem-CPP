#pragma once

#include <string>
#include <vector>
#include "domain/model/Entities.h"

class PersonRepository
{
public:
	PersonRepository() = default;
	virtual ~PersonRepository() = default;
	virtual Person findOrCreateEntry(const std::string& name) = 0;
	virtual std::vector<std::string> getNames() const = 0;
private:
	virtual Person addEntry(const std::string& name) = 0;
};