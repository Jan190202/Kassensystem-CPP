#pragma once

#include "DomainTypes.h"
#include <string>
#include <QDate>

struct BalanceEntry
{
	int64_t BalanceEntryID;
	BalanceType type;
	std::string description;
	double amount;
	QDate date;
	std::string comment;
	int64_t personID;
};

struct ConsumptionEntry
{
	int64_t consumptionEntryID;
	int64_t debtEntryID;
	int nBeer05 = 0, nBeer04 = 0, nSoftdrinks = 0, nWater = 0;
	double otherExpense = 0;
};

struct DebtEntry
{
	int64_t debtEntryID;
	int64_t personID;
	QDate date;
	double amount;
};

struct DebtEntryRemaining
{
	int64_t entryID;
	QDate date;
	double amount;
	double remaining;
};

struct PaymentEntry
{
	int64_t paymentEntryID;
	int64_t personID;
	QDate date;
	double amount;
	OverpaymentDisposition overpaymentType;
};

struct PaymentAllocationEntry
{
	int64_t paymentAllocationEntryID;
	int64_t entryID;
	int64_t paymentID;
	QDate date;
	double amount;
};

struct CreditEntry
{
	int64_t creditEntryID;
	int64_t personID;
	QDate date;
	double amount;
	std::string description;
};

class Person
{
public:
	Person(std::string name, int64_t id);
	std::string getName() const;
	int64_t getID() const;
private:
	std::string name;
	int64_t id;
};