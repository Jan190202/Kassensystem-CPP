#pragma once
#include "domain/model/Entities.h"
#include <string>
#include <expected>
#include <vector>

class PersonRepository
{
public:
	PersonRepository() = default;
	virtual ~PersonRepository() = default;
	virtual int64_t addPersonEntry(entry::Person entry) = 0; // add person entrs, return associated ID

	virtual std::expected<entry::Person,std::string> findPersonEntry(int64_t personID) const = 0; // find person entry by ID
	virtual std::vector<entry::Person> getAllPersonEntries() const = 0; // all person entries in database
};