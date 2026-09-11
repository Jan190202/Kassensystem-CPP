#include "BalanceRepoInMem.h"
#include "IDGenerator.h"
#include <QDebug>

int64_t BalanceRepoInMem::addBalanceEntry(entry::Balance entry)
{
	std::vector<int64_t> usedIDs(entries.size());
	for (size_t i = 0; i < entries.size(); i++)
		usedIDs.at(i) = entries.at(i).balanceEntryID;
	entry.balanceEntryID = idgen::getID(usedIDs);
	
	entries.push_back(entry);

	qInfo() << entry;

	return entry.balanceEntryID;
}

std::vector<entry::Balance> BalanceRepoInMem::getBalanceEntries(BalanceType type, const QDate& minDate) const
{
	if (type == BalanceType::EarningAndSupplement) type = BalanceType::Earning;

	std::vector<entry::Balance> filteredEntries;
	
	for (const auto& entry : entries)
	{
		if (entry.dateBooked < minDate) continue;
		if (type == BalanceType::EarningAndSpending || entry.type == type) filteredEntries.push_back(entry);
	}

	return filteredEntries;
}

std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> BalanceRepoInMem::getBalanceEntry(const std::string& description) const
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