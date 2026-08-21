#pragma once

#include "domain/repoInterface/PersonRepository.h"
#include "domain/model/Entities.h"

#include <string>
#include <vector>

class PersonRepoInMem : public PersonRepository
{
public:
	PersonRepoInMem() = default;
	virtual ~PersonRepoInMem() = default;
	virtual Person findOrCreateEntry(const std::string& name) override;
	virtual std::vector<std::string> getNames() const override;
private:
	virtual Person addEntry(const std::string& name) override;
	std::vector<Person> entries;
	int64_t getUniqueID();
	bool isUnique(int64_t id);
};