#pragma once

#include <string>

class Person
{
public:
	Person(std::string name, int id);
	std::string getName() const;
	int getID() const;
private:
	std::string name;
	int id;
};