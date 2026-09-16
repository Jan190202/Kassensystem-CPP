#pragma once
#include "domain/repointerface/PaymentRepository.h"
#include <vector>

class SqlitePaymentRepository : public PaymentRepository
{
public:
	SqlitePaymentRepository() = default;
	virtual ~SqlitePaymentRepository() = default;
	virtual int64_t addPaymentEntry(const entry::Payment& entry) override;
	virtual int64_t addPaymentAllocationEntry(const entry::PaymentAllocation& entry) override;

	virtual std::vector<entry::PaymentAllocation> getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const override;
	virtual double getTotalAllocatedPayments(const QDate& minDate) const override;
};