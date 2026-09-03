#pragma once

#include <QPushButton>
#include "domain/services/BalanceService.h"
#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include <string>
#include <variant>

enum class BtnIndex
{
	AddEarning, AddSpending
};

struct ServiceBundle
{
	PaymentService& paymentService;
	ConsumptionService& consumptionService;
	BalanceService& balanceService;
	PersonRepository* personRepo;
};

struct LowerButtonBundle
{
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;
};

struct ConsumptionInputs
{
	std::variant<int64_t, std::string> personInput;
	int nBeer05, nBeer04, nSoftdrinks, nWater;
	double otherExpense;
};