#include "DebtRepoInMem.h"
#include "IDGenerator.h"

#include <QDebug>

DebtRepoInMem::DebtRepoInMem(PaymentRepository* paymentRepo, SettlementRepository* settlementRepo) 
	: paymentRepo(paymentRepo), settlementRepo(settlementRepo) {}

int64_t DebtRepoInMem::addEntry(entry::Debt entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i{}; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).debtEntryID;
	entry.debtEntryID = idgen::getID(usedIDs);
	
	entries.push_back(entry);

	qInfo() << entry;

	return entry.debtEntryID;
}

double DebtRepoInMem::getTotal(int64_t personID) const
{
	double amount{};

	for (auto& entry : entries)
	{
		if (entry.personID == personID)
		{
			amount += entry.amount;
		}
	}

	return amount;
}

double DebtRepoInMem::getTotal(FinancialShare share) const
{
	double amount{};

	double currentShare = 1;
	for (auto& entry : entries)
	{
		switch (share)
		{
		case FinancialShare::All:
			currentShare = 1;
			break;
		case FinancialShare::Foreign:
			currentShare = entry.foreignShare;
			break;
		case FinancialShare::Own:
			currentShare = 1 - entry.foreignShare;
			break;
		}

		amount += entry.amount * currentShare;
	}

	return amount;
}

double DebtRepoInMem::getDue(int64_t personID) const
{
	double due{};
	for (const auto& entry : getPaymentOutstandingEntries(personID, FilterType::OmitFullyPaid))
		due+=entry.remaining;
	return due;
}

double DebtRepoInMem::getDue() const
{
	double due{};
	for (const auto& entry : getSettlementOutstandingEntries(FilterType::OmitFullyPaid))
		due += entry.remaining;
	return due;
}

double DebtRepoInMem::getPaid(int64_t personID) const
{
	double settled{};
	for (auto& entry : getPaymentOutstandingEntries(personID, FilterType::IncludeFullyPaid))
		settled += entry.amount - entry.remaining;
	return settled;
}

std::vector<entry::Outstanding> DebtRepoInMem::getPaymentOutstandingEntries(int64_t personID, FilterType filter) const
{
	std::vector<entry::Outstanding> filteredEntries = {};

	for (auto& entry : entries)
	{
		if (entry.personID == personID)
		{
			double remaining = entry.amount - getAllocatedPayments(entry.debtEntryID);

			if (filter == FilterType::OmitFullyPaid && remaining < 1e-9) continue;

			entry::Outstanding entryOut;
			entryOut.debtEntryID = entry.debtEntryID;
			entryOut.date = entry.date;
			entryOut.amount = entry.amount;
			entryOut.remaining = remaining;

			filteredEntries.push_back(entryOut);
		}
	}

	return filteredEntries;
}

std::vector<entry::Outstanding> DebtRepoInMem::getSettlementOutstandingEntries(FilterType filter) const
{
	std::vector<entry::Outstanding> filteredEntries = {};

	for (const auto& entry : entries)
	{
		double remaining = entry.foreignShare * entry.amount -  getAllocatedSettlements(entry.debtEntryID);

		if (filter == FilterType::OmitFullyPaid && remaining < 1e-9) continue;

		entry::Outstanding entryOut;
		entryOut.debtEntryID = entry.debtEntryID;
		entryOut.date = entry.date;
		entryOut.amount = entry.amount;
		entryOut.remaining = remaining;

		filteredEntries.push_back(entryOut);
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

double DebtRepoInMem::getAllocatedSettlements(int64_t debtEntryID) const
{
	std::vector<entry::SettlementAllocation> allocEntries = settlementRepo->getAllocEntries(debtEntryID);

	double allocatedSettlements{};
	for (auto& entry : allocEntries)
		allocatedSettlements += entry.amount;

	return allocatedSettlements;
}