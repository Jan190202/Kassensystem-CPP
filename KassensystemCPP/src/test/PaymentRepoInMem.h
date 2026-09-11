#pragma once
#include "domain/repointerface/PaymentRepository.h"
#include <QDate>
#include <vector>

class PaymentRepoInMem : public PaymentRepository
{
public:
	PaymentRepoInMem() = default;
	virtual ~PaymentRepoInMem() = default;
	virtual int64_t addPaymentEntry(entry::Payment) override;
	virtual int64_t addPaymentAllocationEntry(entry::PaymentAllocation) override;

	virtual std::vector<entry::PaymentAllocation> getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const override;
	virtual double getTotalAllocatedPayments(const QDate& minDate) const override;

private:
	entry::Payment getPaymentEntry(int64_t paymentEntryID) const;

	std::vector<entry::Payment> paymentEntries{};
	std::vector<entry::PaymentAllocation> paymentAllocationEntries{};
};