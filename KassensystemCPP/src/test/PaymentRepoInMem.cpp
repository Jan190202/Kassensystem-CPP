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

	qInfo() << "entry::Payment added! Entries: ";
	for (auto& entry : paymentEntries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " date:" << entry.date
			<< " overpaymentType:" << static_cast<int>(entry.overpaymentType)
			<< " paymentID:" << entry.paymentEntryID
			<< " personID:" << entry.personID;
	}

	return entry.paymentEntryID;
}

int64_t PaymentRepoInMem::addAllocationEntry(entry::PaymentAllocation entry)
{
	std::vector<int64_t> usedIDs(paymentAllocationEntries.size());
	for (size_t i = 0; i < paymentAllocationEntries.size(); i++)
		usedIDs.at(i) = paymentAllocationEntries.at(i).paymentAllocationEntryID;
	entry.paymentAllocationEntryID = idgen::getID(usedIDs);
	paymentAllocationEntries.emplace_back(entry);

	qInfo() << "entry::PaymentAllocation added! Entries: ";
	for (auto& entry : paymentAllocationEntries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " date:" << entry.date
			<< " debtEntryID:" << entry.debtEntryID
			<< " paymentAllocationID:" << entry.paymentAllocationEntryID
			<< " paymentEntryID:" << entry.paymentEntryID;
	}
	return entry.paymentAllocationEntryID;
}

std::vector<entry::PaymentAllocation> PaymentRepoInMem::getAllocEntries(int64_t debtEntryID) const
{
	std::vector<entry::PaymentAllocation> filteredEntries{};

	for (auto& allocEntry : paymentAllocationEntries)
	{
		if (allocEntry.debtEntryID == debtEntryID)
			filteredEntries.push_back(allocEntry);
	}

	return filteredEntries;
}

//double PaymentRepoInMem::getPaidAmount(int64_t personID) const
//{
//	double paidAmount = 0;
//	
//	for (auto& entry : paymentEntries)
//	{
//		if (entry.personID == personID)
//		{
//			paidAmount += entry.amount;
//		}
//	}
//
//	return paidAmount;
//}
