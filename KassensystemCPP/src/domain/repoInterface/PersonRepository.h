#pragma once
#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"
#include <string>
#include <expected>
#include <vector>

class PersonRepository
{
public:
	PersonRepository() = default;
	virtual ~PersonRepository() = default;
	virtual int64_t addPersonEntry(const entry::Person& entry) = 0; // add person entrs, return associated ID

	virtual std::expected<entry::Person,GetEntryException> findPersonEntry(int64_t personEntryID) const = 0; // find person entry by ID
	virtual std::vector<entry::Person> getAllPersonEntries() const = 0; // all person entries in database
};