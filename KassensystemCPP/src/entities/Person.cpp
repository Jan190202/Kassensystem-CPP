#include "Person.h"
#include <string>

Person::Person(std::string name, int id)
{
	this->name = name;
	this->id = id;
}

std::string Person::getName() const
{
	return name;
}

int Person::getID() const
{
	return id;
}