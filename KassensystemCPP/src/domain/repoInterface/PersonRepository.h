#pragma once

#include <string>
#include <expected>
#include <vector>
#include "domain/model/Entities.h"

class PersonRepository
{
public:
	PersonRepository() = default;
	virtual ~PersonRepository() = default;

	virtual std::expected<Person,std::string> findEntry(int64_t personID) = 0;
	virtual Person addEntry(const std::string& firstName, const std::string& lastName, const std::string& nickName = "", const std::string& info = "") = 0;

	virtual std::vector<Person> getAll() const = 0;
};