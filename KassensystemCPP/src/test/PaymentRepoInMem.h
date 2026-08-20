#pragma once

#include "domain/repoInterface/PaymentRepository.h"
#include <QDate>
#include <vector>

class PaymentRepoInMem : public PaymentRepository
{
public:
	PaymentRepoInMem() = default;
	virtual ~PaymentRepoInMem() = default;

	virtual void addPayment(int nameID, double amount, QDate date) override;
	virtual void addPaymentAllocation(int entryID, int paymentID, double amount, QDate date) override;
	virtual double getPaidAmount(int nameID) const override;

private:
	std::vector<PaymentEntry> paymentEntries = {};
	std::vector<PaymentAllocationEntry> paymentAllocationEntries = {};
};