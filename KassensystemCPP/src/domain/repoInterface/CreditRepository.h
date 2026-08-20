#pragma once

#include <QDate>;

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;

	virtual void addCredit(int personID, double amount, QDate date = QDate::currentDate()) = 0;
	virtual double getCredit(int personID) const = 0;
};