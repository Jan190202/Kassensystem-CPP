#pragma once

#include "domain/repoInterface/PaymentRepository.h"
#include <QDate>
#include <vector>

class PaymentRepoInMem : public PaymentRepository
{
public:
	PaymentRepoInMem() = default;
	virtual ~PaymentRepoInMem() = default;

	virtual int64_t addPaymentEntry(const entry::Payment&) override;
	virtual int64_t addAllocationEntry(const entry::PaymentAllocation&) override;
	//virtual double getPaidAmount(int64_t personID) const override;

private:
	std::vector<entry::Payment> paymentEntries = {};
	std::vector<entry::PaymentAllocation> paymentAllocationEntries = {};
};