#pragma once

#include "domain/repoInterface/PaymentRepository.h"
#include <QDate>
#include <vector>

class PaymentRepoInMem : public PaymentRepository
{
public:
	PaymentRepoInMem() = default;
	virtual ~PaymentRepoInMem() = default;

	virtual int64_t addPaymentEntry(entry::Payment) override;
	virtual int64_t addAllocationEntry(entry::PaymentAllocation) override;
	//virtual double getPaidAmount(int64_t personID) const override;

	virtual std::vector<entry::PaymentAllocation> getAllocEntries(int64_t debtEntryID) const override;

private:
	std::vector<entry::Payment> paymentEntries = {};
	std::vector<entry::PaymentAllocation> paymentAllocationEntries = {};
};