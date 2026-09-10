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
	void resetCredit(int64_t personID);

	double getSettledAmount(int64_t personID) const;
	double getTotalAmount(int64_t personID) const;
	double getDueAmount(int64_t personID) const;
	double getCreditAmount(int64_t personID) const;
	std::vector<entry::Consumption> getConsumptionEntries(int64_t personID) const;
	std::vector<entry::Outstanding> getPaymentOutstandingEntries(int64_t personID, FilterType filter) const;

private:
	double addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount);
	int64_t addTip(int64_t personID, double amount, QDate date);
	int64_t addCredit(int64_t personID, double amount, QDate date, std::string description);

	PaymentRepository* paymentRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	ConsumptionRepository* consumptionRepo;
	BalanceRepository* balanceRepo;
	PersonRepository* personRepo;
};