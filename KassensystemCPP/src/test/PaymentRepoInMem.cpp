#include "PaymentRepoInMem.h"
#include "IDGenerator.h"
#include <QDebug>

int64_t PaymentRepoInMem::addPaymentEntry(entry::Payment entry)
{
	std::vector<int64_t> usedIDs(paymentEntries.size());
	for (size_t i = 0; i < paymentEntries.size(); i++)
		usedIDs.at(i) = paymentEntries.at(i).paymentEntryID;
	entry.paymentEntryID = idgen::getID(usedIDs);

	paymentEntries.emplace_back(entry);

	qInfo() << entry;

	return entry.paymentEntryID;
}

int64_t PaymentRepoInMem::addPaymentAllocationEntry(entry::PaymentAllocation entry)
{
	std::vector<int64_t> usedIDs(paymentAllocationEntries.size());
	for (size_t i = 0; i < paymentAllocationEntries.size(); i++)
		usedIDs.at(i) = paymentAllocationEntries.at(i).paymentAllocationEntryID;
	entry.paymentAllocationEntryID = idgen::getID(usedIDs);
	paymentAllocationEntries.emplace_back(entry);

	qInfo() << entry;

	return entry.paymentAllocationEntryID;
}

std::vector<entry::PaymentAllocation> PaymentRepoInMem::getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const
{
	std::vector<entry::PaymentAllocation> filteredEntries{};

	for (const auto& allocEntry : paymentAllocationEntries)
	{
		if (allocEntry.debtEntryID == debtEntryID)
			filteredEntries.push_back(allocEntry);
	}

	return filteredEntries;
}

double PaymentRepoInMem::getTotalAllocatedPayments() const
{
	double paidTotal{};

	for (const auto& entry : paymentAllocationEntries)
		paidTotal += entry.amount;

	return paidTotal;
}