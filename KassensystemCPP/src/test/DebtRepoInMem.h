#pragma once
#include "domain/repointerface/DebtRepository.h"
#include "domain/repointerface/PaymentRepository.h"
#include "domain/repointerface/ShareSettlementRepository.h"
#include <vector>

class DebtRepoInMem : public DebtRepository
{
public:
	DebtRepoInMem(PaymentRepository* paymentRepo, ShareSettlementRepository* shareSettlementRepo);
	virtual ~DebtRepoInMem() = default;
	virtual int64_t addDebtEntry(entry::Debt entry) override;

	virtual double getPersonsTotal(int64_t personEntryID) const override;
	virtual double getPersonsDue(int64_t personEntryID) const override;
	virtual double getPersonsPaid(int64_t personEntryID) const override;
	virtual double getTotalShare(FinancialShare share, const QDate& minDate) const override;
	virtual double getForeignDue() const override;
	virtual std::vector<entry::Outstanding> getPersonsOutstandingEntries(int64_t personEntryID, FilterType filter) const override;
	virtual std::vector<entry::Outstanding> getForeignShareOutstandingEntries(FilterType filter) const override;
private:
	std::vector<entry::Debt> entries{};
	double getAllocatedPayments(int64_t debtEntryID) const;
	double getAllocatedShareSettlements(int64_t debtEntryID) const;

	PaymentRepository* paymentRepo;
	ShareSettlementRepository* shareSettlementRepo;
};