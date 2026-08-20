#pragma once

#include <string>
#include <QDate>
#include "domain/DomainTypes.h"

struct BalanceEntry
{
	BalanceType type;
	std::string description;
	double amount;
	QDate date;
	std::string comment;
	int personID;
};