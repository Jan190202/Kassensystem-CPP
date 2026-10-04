#pragma once
#include "domain/model/Entities.h"
#include "app/RepositoryBundle.h"
#include <string>
#include <variant>
#include <expected>

namespace validityError
{
	enum class Name
	{
		Empty,
		FirstOrLastNameMissing,
		UnbalancedParentheses,
		InvalidNicknameFormat,
		TooManyComponents
	};
}

class PersonService
{
public:
	struct PersonStringSpecifiers
	{
		std::string firstName, lastName, nickName, info;
	};

	PersonService(const RepositoryBundle& repoBundle, PendingChangeLog& log);

	std::expected< int64_t, validityError::Name > findOrCreatePerson(const std::variant<int64_t, std::string>& personInput); // if variant has ID -> get 
	int64_t addPerson(const entry::Person& person);

	std::expected< PersonStringSpecifiers, validityError::Name > isValidNameFormat(const std::string& nameRequest) const;

private:
	PersonRepository* personRepo;
	PendingChangeLog& log;
};