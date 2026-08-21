#pragma once

#include "domain/repoInterface/PaymentRepository.h"
#include <QDate>
#include <vector>

class PaymentRepoInMem : public PaymentRepository
{
public:
	PaymentRepoInMem() = default;
	virtual ~PaymentRepoInMem() = default;

	virtual int64_t addPaymentEntry(const PaymentEntry&) override;
	virtual int64_t addAllocationEntry(const PaymentAllocationEntry&) override;
	virtual double getPaidAmount(int64_t personID) const override;

private:
	std::vector<PaymentEntry> paymentEntries = {};
	std::vector<PaymentAllocationEntry> paymentAllocationEntries = {};
};