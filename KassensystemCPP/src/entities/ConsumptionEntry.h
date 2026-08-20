#pragma once

#include <QDate>
#include "Person.h"

struct ConsumptionEntry
{
	QDate date;
	int personID;
	int nBeer05, nBeer04, nSoftdrinks, nWater;
	double otherExpense;
};