#pragma once

#include "domain/repoInterface/DebtRepository.h"
#include "domain/repoInterface/PaymentRepository.h"
#include "domain/repoInterface/SettlementRepository.h"
#include <vector>

class DebtRepoInMem : public DebtRepository
{
public:
	DebtRepoInMem(PaymentRepository* paymentRepo, SettlementRepository* settlementRepo);
	virtual ~DebtRepoInMem() = default;

	virtual int64_t addEntry(entry::Debt entry) override;
	virtual double getTotal(FinancialShare share) const override;
	virtual double getTotal(int64_t personID) const override;
	virtual double getDue(int64_t personID) const override;
	virtual double getDue() const override;
	virtual double getPaid(int64_t personID) const override;
	virtual std::vector<entry::Outstanding> getPaymentOutstandingEntries(int64_t personID, FilterType filter) const override;
	virtual std::vector<entry::Outstanding> getSettlementOutstandingEntries(FilterType filter) const override;
private:
	std::vector<entry::Debt> entries;
	double getAllocatedPayments(int64_t debtEntryID) const;
	double getAllocatedSettlements(int64_t debtEntryID) const;

	PaymentRepository* paymentRepo;
	SettlementRepository* settlementRepo;
};