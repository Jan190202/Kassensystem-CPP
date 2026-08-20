#pragma once

#include <string>
#include <QDate>
#include "Person.h"
#include "domain/DomainTypes.h"

struct BalanceEntry
{
	BalanceType type;
	std::string description;
	double amount;
	QDate date;
	std::string comment;
	Person coveringPerson;
};