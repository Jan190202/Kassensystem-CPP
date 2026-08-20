#include "PaymentRepoInMem.h"

void PaymentRepoInMem::addPayment(int nameID, double amount, QDate date)
{
	paymentEntries.emplace_back(PaymentEntry{ nameID, amount, date });
}

void PaymentRepoInMem::addPaymentAllocation(int entryID, int paymentID, double amount, QDate date)
{
	paymentAllocationEntries.emplace_back(PaymentAllocationEntry{ entryID, paymentID, amount, date });
}

double PaymentRepoInMem::getPaidAmount(int nameID) const
{
	double paidAmount = 0;
	
	for (auto& entry : paymentEntries)
	{
		if (entry.nameID == nameID)
		{
			paidAmount += entry.amount;
		}
	}

	return paidAmount;
}
