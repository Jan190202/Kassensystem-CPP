#pragma once

#include "entities/Person.h"
#include <QDate>;

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;

	virtual void addCredit(const Person& person, double amount, QDate date = QDate::currentDate()) = 0;
	virtual double getCredit(const Person& person) const = 0;
};