#pragma once

#include "domain/model/Entities.h"
#include <QDate>

class PaymentRepository
{
public:
	PaymentRepository() = default;
	virtual ~PaymentRepository() = default;
	virtual int64_t addPaymentEntry(const PaymentEntry&) = 0;
	virtual int64_t addAllocationEntry(const PaymentAllocationEntry&) = 0;
	virtual double getPaidAmount(int64_t personID) const = 0;
};