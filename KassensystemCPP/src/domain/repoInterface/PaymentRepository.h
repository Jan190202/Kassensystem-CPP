#pragma once

#include "domain/model/Entities.h"

#include <vector>

#include <QDate>

class PaymentRepository
{
public:
	PaymentRepository() = default;
	virtual ~PaymentRepository() = default;
	virtual int64_t addPaymentEntry(entry::Payment) = 0;
	virtual int64_t addAllocationEntry(entry::PaymentAllocation) = 0;
	//virtual double getPaidAmount(int64_t personID) const = 0;

	virtual std::vector<entry::PaymentAllocation> getAllocEntries(int64_t debtEntryID) const = 0;
};