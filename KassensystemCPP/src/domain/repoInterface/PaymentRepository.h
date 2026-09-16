#pragma once
#include "domain/model/Entities.h"
#include <vector>

class PaymentRepository
{
public:
	PaymentRepository() = default;
	virtual ~PaymentRepository() = default;
	virtual int64_t addPaymentEntry(const entry::Payment& entry) = 0; // add payment entry to database, return associated ID
	virtual int64_t addPaymentAllocationEntry(const entry::PaymentAllocation& entry) = 0; // add payment allocation entry to database, return associated ID

	virtual std::vector<entry::PaymentAllocation> getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const = 0; // payment allocation entries associated with debt entry
	virtual double getTotalAllocatedPayments(const QDate& minDate) const = 0; // total payments belonging to debts in database, where payments came after or at minDate (e.g. excluding tips after overpayment)
};