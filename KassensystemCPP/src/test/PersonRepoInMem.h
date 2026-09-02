#pragma once

#include "domain/repoInterface/PersonRepository.h"
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

	virtual std::expected<Person, std::string> findEntry(int64_t personID) override;
	virtual Person addEntry(const std::string& firstName, const std::string& lastName, const std::string& nickName = "", const std::string& info = "") override;

	virtual std::vector<Person> getAll() const override;

private:
	std::vector<Person> entries;
	int64_t getUniqueID();
	bool isUnique(int64_t id);
};