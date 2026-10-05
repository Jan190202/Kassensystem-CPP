#pragma once
#include "app/RepositoryBundle.h"
#include "domain/model/DomainTypes.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"

class PaymentService
{
public:
	PaymentService(const RepositoryBundle& repoBundle, PendingChangeLog& log);
	void addPayment(const request::Payment& request);
	int64_t addCredit(const request::Credit& request);
	std::vector<exportType::PersonDebt> getDebtsAll() const;
private:
	double addPaymentAllocation(int64_t paymentEntryID, int64_t personEntryID, double amount);
	int64_t addCredit(const entry::Person& person, double amount, const RegisterDate& date, const std::string& description);
	int64_t addTip(const entry::Person& person, double amount, const RegisterDate& date);
	
	PaymentRepository* paymentRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	ConsumptionRepository* consumptionRepo;
	BalanceRepository* balanceRepo;
	PersonRepository* personRepo;
	PendingChangeLog& log;
};