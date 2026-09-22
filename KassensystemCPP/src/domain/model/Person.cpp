#include "Entities.h"

using namespace entry;

std::string Person::getFirstName() const
{
	return firstName;
}

std::string Person::getLastName() const
{
	return lastName;
}

std::string Person::getFullName() const
{
	bool isFirst = !firstName.empty();
	bool isLast = !lastName.empty();

	if (isFirst && isLast) return firstName + " " + lastName;
	else if (isFirst) return firstName;
	else if (isLast) return lastName;
	else return "NAME UNSPECIFIED";
}

std::string Person::getNickName() const
{
	return nickName;
}

std::string Person::getInfo() const
{
	return info;
}

std::string Person::getFullSpecifier() const
{
	std::string additionalString;

	bool isNickName = !nickName.empty();
	bool isInfo = !info.empty();

	if (isNickName && isInfo) additionalString = " (\"" + nickName + "\"," + info + ")";
	else if (isNickName) additionalString = "(\"" + nickName + "\")";
	else if (isInfo) additionalString = "(" + info + ")";

	if (!additionalString.empty()) additionalString = " " + additionalString;

	return getFullName() + additionalString;
}