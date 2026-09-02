#include "Entities.h"

Person::Person(const std::string& firstName, const std::string& lastName, int64_t id, const std::string& nickName, const std::string& info)
	: firstName(firstName), lastName(lastName), id(id), nickName(nickName), info(info) {}

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
	return firstName + " " + lastName;
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

	bool isNickName = nickName.size() > 0;
	bool isInfo = info.size() > 0;

	if (isNickName && isInfo) additionalString = " (\"" + nickName + "\"," + info + ")";
	else if (isNickName) additionalString = "(\"" + nickName + "\")";
	else if (isInfo) additionalString = "(" + info + ")";

	return getFullName() + " " + additionalString;
}

int64_t Person::getID() const
{
	return id;
}