#pragma once

#include <QDate>

struct PaymentEntry
{
	int nameID = 0;
	double amount = 0;
	QDate date = QDate();
};

struct PaymentAllocationEntry
{
	int entryID = 0, paymentID = 0;
	double amount = 0;
	QDate date;
};

class PaymentRepository
{
public:
	PaymentRepository() = default;
	virtual ~PaymentRepository() = default;
	virtual void addPayment(int nameID, double amount, QDate date) = 0;
	virtual void addPaymentAllocation(int entryID, int paymentID, double amount, QDate date) = 0;
	virtual double getPaidAmount(int nameID) const = 0;
};