#pragma once

#include "DomainTypes.h"
#include <string>
#include <QDate>

namespace entry
{
	struct Balance
	{
		int64_t balanceEntryID;
		BalanceType type;
		std::string description;
		double amount;
		QDate date;
		std::string comment;
		int64_t personID;
	};

	struct Consumption
	{
		int64_t consumptionEntryID;
		int64_t debtEntryID;
		int nBeer05, nBeer04, nSoftdrinks, nWater;
		double otherExpense;
	};

	struct Debt
	{
		int64_t debtEntryID;
		int64_t personID;
		QDate date;
		double amount;
	};

	struct DebtRemaining
	{
		int64_t debtEntryID;
		QDate date;
		double amount;
		double remaining;
	};

	struct Payment
	{
		int64_t paymentEntryID;
		int64_t personID;
		QDate date;
		double amount;
		OverpaymentDisposition overpaymentType;
	};

	struct PaymentAllocation
	{
		int64_t paymentAllocationEntryID;
		int64_t debtEntryID;
		int64_t paymentEntryID;
		QDate date;
		double amount;
	};

	struct Credit
	{
		int64_t creditEntryID;
		int64_t personID;
		QDate date;
		double amount;
		std::string description;
	};
}

class Person
{
public:
	Person(const std::string& firstName, const std::string& lastName, int64_t id, const std::string& nickName = "", const std::string& info = "");
	std::string getFirstName() const;
	std::string getLastName() const;
	std::string getFullName() const;
	std::string getNickName() const;
	std::string getInfo() const;
	std::string getFullSpecifier() const;
	int64_t getID() const;
private:
	std::string firstName, lastName, nickName, info;
	int64_t id;
};