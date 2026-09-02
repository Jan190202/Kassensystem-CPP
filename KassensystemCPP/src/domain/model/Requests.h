#pragma once

#include <string>
#include <variant>
#include <optional>
#include <QDate>

struct ConsumptionRequest
{
	std::variant<int64_t, std::string> personInput;
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
	std::optional<int64_t> coveringPersonID;
};

struct PaymentRequest
{
	int64_t personID;
	QDate date;
	double amount;
	OverpaymentDisposition overpaymentType;
};