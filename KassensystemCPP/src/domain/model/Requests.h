#pragma once
#include "domain/model/Entities.h"
#include <variant>
#include <optional>
#include <QDate>
#include <string>

namespace request
{
	struct Consumption
	{
		std::variant<int64_t, std::string> personInput;
		RegisterDate dateBooked;
		int nBeer05 = 0, nBeer04 = 0, nSoftdrinks = 0, nWater = 0;
		double otherExpense = 0;
	};

	struct Balance
	{
		BalanceType type;
		std::string description;
		double amount;
		RegisterDate dateBooked;
		std::string comment;
		std::optional<int64_t> coveringpersonEntryID;
	};

	struct Payment // specifically no date, as allocation only ever done using the current debt entries, aren't changed afterwards in an earlier payment comes in
	{
		int64_t personEntryID;
		double amount;
		std::string comment;
		OverpaymentDisposition overpaymentType;
	};

	struct ShareSettlement // specifically no date, as allocation only ever done using the current debt entries, aren't changed afterwards in an earlier settlement comes in
	{
		double amount;
		std::string comment;
	};
}