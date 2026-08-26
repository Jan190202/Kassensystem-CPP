#pragma once

#include <QPushButton>
#include "domain/services/BalanceService.h"
#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include <string>
#include <ostream>

enum class btnIndex
{
	addEarning, addSpending
};

struct ServiceBundle
{
	PaymentService& paymentService;
	ConsumptionService& consumptionService;
	BalanceService& balanceService;
};

struct LowerButtonBundle
{
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;
};

struct ConsumptionInputs
{
	std::string personName;
	int nBeer05, nBeer04, nSoftdrinks, nWater;
	double otherExpense;
};

std::ostream& operator<<(std::ostream& out, const ConsumptionInputs& inputs);