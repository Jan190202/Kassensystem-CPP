#include "PaymentRepoInMem.h"

int64_t PaymentRepoInMem::addPaymentEntry(const PaymentEntry& entry)
{
	paymentEntries.emplace_back(entry);
	return 0;
}

int64_t PaymentRepoInMem::addAllocationEntry(const PaymentAllocationEntry& entry)
{
	paymentAllocationEntries.emplace_back(entry);
	return 0;
}

double PaymentRepoInMem::getPaidAmount(int64_t personID) const
{
	double paidAmount = 0;
	
	for (auto& entry : paymentEntries)
	{
		if (entry.personID == personID)
		{
			paidAmount += entry.amount;
		}
	}

	return paidAmount;
}
