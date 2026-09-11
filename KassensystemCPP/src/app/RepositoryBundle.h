#pragma once

#include "domain/repointerface/PersonRepository.h"
#include "domain/repointerface/ConsumptionRepository.h"
#include "domain/repointerface/DebtRepository.h"
#include "domain/repointerface/PaymentRepository.h"
#include "domain/repointerface/CreditRepository.h"
#include "domain/repointerface/BalanceRepository.h"
#include "domain/repointerface/ShareSettlementRepository.h"

struct RepositoryBundle
{
	PersonRepository*			personRepo;
	ConsumptionRepository*		consumptionRepo;
	DebtRepository*				debtRepo;
	PaymentRepository*			paymentRepo;
	CreditRepository*			creditRepo;
	BalanceRepository*			balanceRepo;
	ShareSettlementRepository*	shareSettlementRepo;
};