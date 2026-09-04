#include "DebtRepoInMem.h"
#include "IDGenerator.h"

#include <QDebug>

DebtRepoInMem::DebtRepoInMem(PaymentRepository* paymentRepo) : paymentRepo(paymentRepo) {}

int64_t DebtRepoInMem::addEntry(entry::Debt entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).debtEntryID;
	entry.debtEntryID = idgen::getID(usedIDs);
	
	entries.push_back(entry);

	qInfo() << "entry::Debt added! Entries: ";
	for (auto& entry : entries)
	{
		qInfo()
			<< " amount:" << entry.amount
			<< " date:" << entry.date
			<< " debtID:" << entry.debtEntryID
			<< " personID:" << entry.personID;
	}

	return entry.debtEntryID;
}

double DebtRepoInMem::getTotal(int64_t personID) const
{
	double amount = 0;

	for (auto& entry : entries)
	{
		if (entry.personID == personID)
		{
			amount += entry.amount;
		}
	}

	return amount;
}

double DebtRepoInMem::getDue(int64_t personID) const
{
	double due{ 0 };
	for (auto& entry : getOutstandingEntries(personID, FilterType::OmitFullyPaid))
		due+=entry.remaining;
	return due;
}

double DebtRepoInMem::getSettled(int64_t personID) const
{
	double settled{ 0 };
	for (auto& entry : getOutstandingEntries(personID, FilterType::IncludeFullyPaid))
		settled += entry.amount - entry.remaining;
	return settled;
}

std::vector<entry::DebtRemaining> DebtRepoInMem::getOutstandingEntries(int64_t personID, FilterType filter) const
{
	std::vector<entry::DebtRemaining> filteredEntries = {};

	for (auto& entry : entries)
	{
		if (entry.personID == personID)
		{
			double remaining = entry.amount - getAllocatedPayments(entry.debtEntryID);

			if (filter == FilterType::OmitFullyPaid && remaining < 1e-9) continue;

			entry::DebtRemaining entryRem;
			entryRem.debtEntryID = entry.debtEntryID;
			entryRem.date = entry.date;
			entryRem.amount = entry.amount;
			entryRem.remaining = remaining;

			filteredEntries.push_back(entryRem);
		}
	}

	return filteredEntries;
}

double DebtRepoInMem::getAllocatedPayments(int64_t debtEntryID) const
{
	std::vector<entry::PaymentAllocation> allocEntries = paymentRepo->getAllocEntries(debtEntryID);

	double allocatedPayments{};
	for (auto& entry : allocEntries)
		allocatedPayments += entry.amount;

	return allocatedPayments;
}