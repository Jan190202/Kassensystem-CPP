#include "BalanceRepoInMem.h"
#include "IDGenerator.h"

#include <QDebug>

int64_t BalanceRepoInMem::addEntry(entry::Balance entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).balanceEntryID;
	entry.balanceEntryID = idgen::getID(usedIDs);
	
	entries.push_back(entry);

	qInfo() << entry;

	return entry.balanceEntryID;
}

//double BalanceRepoInMem::getTotalEarnings() const
//{
//	double totalEarnings = 0;
//
//	for (auto& entry : entries)
//	{
//		if (entry.type == BalanceType::Earning)
//		{
//			totalEarnings += entry.amount;
//		}
//	}
//
//	return totalEarnings;
//}
//
//double BalanceRepoInMem::getTotalSpendings() const
//{
//	double totalSpendings = 0;
//
//	for (auto& entry : entries)
//	{
//		if (entry.type == BalanceType::Spending)
//		{
//			totalSpendings += entry.amount;
//		}
//	}
//
//	return totalSpendings;
//}

std::vector<entry::Balance> BalanceRepoInMem::getEntries(BalanceType type) const
{
	if (type == BalanceType::EarningAndSpending) return entries;

	if (type == BalanceType::EarningAndSupplement) type = BalanceType::Earning;
	
	std::vector<entry::Balance> filteredEntries;

	for (auto& entry : entries)
	{
		if (entry.type == type) filteredEntries.push_back(entry);
	}

	return filteredEntries;
}

std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> BalanceRepoInMem::getEntry(std::string description) const
{
	const entry::Balance* foundEntry = nullptr;

	for (auto& entry : entries)
	{
		if (entry.description == description)
		{
			if (foundEntry) return std::unexpected(GetEntryException::MultipleEntriesFound);

			foundEntry = &entry;
		}
	}

	if (foundEntry) return std::ref(*foundEntry);
	else return std::unexpected(GetEntryException::EntryNotFound);
}