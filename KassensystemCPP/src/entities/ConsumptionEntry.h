#pragma once

#include <QDate>

struct ConsumptionEntry
{
	QDate date = QDate();
	int personID = 0;
	int nBeer05 = 0, nBeer04 = 0, nSoftdrinks = 0, nWater = 0;
	double otherExpense = 0;
};