#pragma once

#include <QDate>
#include "Person.h"

struct ConsumptionEntry
{
	QDate date;
	Person person;
	int nBeer05, nBeer04, nSoftdrinks, nWater;
	double otherExpense;
};