#pragma once
#include "domain/repointerface/PersonRepository.h"
#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"
#include <string>
#include <expected>
#include <vector>

class PersonRepoInMem : public PersonRepository
{
public:
	PersonRepoInMem() = default;
	virtual ~PersonRepoInMem() = default;
	virtual int64_t addPersonEntry(entry::Person entry) override;

	virtual std::expected<entry::Person, std::string> findPersonEntry(int64_t personEntryID) const override;
	virtual std::vector<entry::Person> getAllPersonEntries() const override;

private:
	std::vector<entry::Person> entries{};
};