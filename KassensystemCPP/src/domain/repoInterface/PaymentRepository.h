#pragma once
#include "domain/model/Entities.h"
#include <QDate>
#include <vector>

class PaymentRepository
{
public:
	PaymentRepository() = default;
	virtual ~PaymentRepository() = default;
	virtual int64_t addPaymentEntry(const entry::Payment& entry) = 0; // add payment entry to database, return associated ID
	virtual int64_t addPaymentAllocationEntry(const entry::PaymentAllocation& entry) = 0; // add payment allocation entry to database, return associated ID

	virtual std::vector<entry::PaymentAllocation> getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const = 0; // payment allocation entries associated with debt entry
	virtual double getTotalAllocatedPayments(const DateRange& range) const = 0; // total payments belonging to debts (e.g. excluding tips after overpayment), for payments added within range (filters on the payment's dateAdded: that is when the cash moved)
};