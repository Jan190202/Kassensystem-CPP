#include "Entities.h"

Person::Person(std::string name, int64_t id)
{
	this->name = name;
	this->id = id;
}

std::string Person::getName() const
{
	return name;
}

int64_t Person::getID() const
{
	return id;
}