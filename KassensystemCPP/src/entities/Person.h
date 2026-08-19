#pragma once

#include <string>

class Person
{
public:
	Person(std::string name);

private:
	std::string name;
	int id;
};