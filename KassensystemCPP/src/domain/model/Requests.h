#pragma once

#include <string>
#include <QDate>

struct ConsumptionRequest
{
	std::string personName;
	QDate date;
	int nBeer05 = 0, nBeer04 = 0, nSoftdrinks = 0, nWater = 0;
	double otherExpense = 0;
};

struct BalanceRequest
{
	BalanceType type;
	std::string description;
	double amount;
	QDate date;
	std::string comment;
	bool isCovered;
	std::string personName;
};

struct PaymentRequest
{
	std::string personName;
	QDate date;
	double amount;
	OverpaymentDisposition overpaymentType;
};