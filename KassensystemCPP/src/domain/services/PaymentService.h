#pragma once

#include "domain/repoInterface/PaymentRepository.h"
#include "domain/repoInterface/CreditRepository.h"
#include "domain/repoInterface/DebtRepository.h"
#include "domain/repoInterface/BalanceRepository.h"
#include "domain/repoInterface/PersonRepository.h"
#include "domain/model/DomainTypes.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"

class PaymentService
{
public:
	PaymentService(PaymentRepository*, CreditRepository*, DebtRepository*, BalanceRepository*, PersonRepository*);
	
	void addPayment(const PaymentRequest& request);
	double getPaidAmount(int64_t personID) const;
	double getTotalAmount(int64_t personID) const;
	double getCreditAmount(int64_t personID) const;
private:
	double addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount, QDate date);
	int64_t addTip(int64_t personID, double amount, QDate date);
	int64_t addCredit(int64_t personID, double amount, QDate date, std::string description);

	PaymentRepository* paymentRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	BalanceRepository* balanceRepo;
	PersonRepository* personRepo;
};