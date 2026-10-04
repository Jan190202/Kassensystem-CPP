#pragma once

#include "domain/services/BalanceService.h"
#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include "domain/services/PersonService.h"

struct ServiceBundle
{
	ConsumptionService& consumptionService;
	PaymentService&		paymentService;
	BalanceService&		balanceService;
	PersonService&		personService;
};