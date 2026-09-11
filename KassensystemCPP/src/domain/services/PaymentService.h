#pragma once
#include "app/RepositoryBundle.h"
#include "domain/model/DomainTypes.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"

class PaymentService
{
public:
	PaymentService(const RepositoryBundle& repoBundle);
	void addPayment(const request::Payment& request);

private:
	double addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount);
	int64_t addTip(int64_t personID, double amount, const QDate& date);
	int64_t addCredit(int64_t personID, double amount, const QDate& date, const std::string& description);

	PaymentRepository* paymentRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	ConsumptionRepository* consumptionRepo;
	BalanceRepository* balanceRepo;
	PersonRepository* personRepo;
};