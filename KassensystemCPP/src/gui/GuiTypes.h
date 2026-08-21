#pragma once

#include "domain/services/BalanceService.h"
#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"

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
