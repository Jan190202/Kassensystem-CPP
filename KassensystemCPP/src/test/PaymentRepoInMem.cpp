#include "PaymentRepoInMem.h"

#include <QDebug>

int64_t PaymentRepoInMem::addPaymentEntry(const PaymentEntry& entry)
{
	paymentEntries.emplace_back(entry);

	qInfo() << "PaymentEntry added! Entries: ";
	for (auto& entry : paymentEntries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " date:" << entry.date
			<< " overpaymentType:" << static_cast<int>(entry.overpaymentType)
			<< " paymentID:" << entry.paymentEntryID
			<< " personID:" << entry.personID;
	}

	return 0;
}

int64_t PaymentRepoInMem::addAllocationEntry(const PaymentAllocationEntry& entry)
{
	paymentAllocationEntries.emplace_back(entry);

	qInfo() << "PaymentAllocationEntry added! Entries: ";
	for (auto& entry : paymentAllocationEntries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " date:" << entry.date
			<< " entryID:" << entry.entryID
			<< " paymentAllocationID:" << entry.paymentAllocationEntryID
			<< " paymentID:" << entry.paymentID;
	}
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
