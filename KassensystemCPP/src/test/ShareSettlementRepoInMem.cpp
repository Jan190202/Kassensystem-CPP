#include "ShareSettlementRepoInMem.h"
#include "IDGenerator.h"

int64_t ShareSettlementRepoInMem::addShareSettlementEntry(entry::ShareSettlement entry)
{
	std::vector<int64_t> usedIDs(shareSettlementEntries.size());
	for (size_t i = 0; i < shareSettlementEntries.size(); i++) 
		usedIDs.at(i) = shareSettlementEntries.at(i).shareSettlementEntryID;
	entry.shareSettlementEntryID = idgen::getID(usedIDs);
	
	shareSettlementEntries.push_back(entry);

	qInfo() << entry;

	return entry.shareSettlementEntryID;
}

int64_t ShareSettlementRepoInMem::addShareSettlementAllocationEntry(entry::ShareSettlementAllocation entry)
{
	std::vector<int64_t> usedIDs(shareSettlementAllocationEntries.size());
	for (size_t i = 0; i < shareSettlementAllocationEntries.size(); i++) 
		usedIDs.at(i) = shareSettlementAllocationEntries.at(i).shareSettlementEntryID;
	entry.shareSettlementAllocationEntryID = idgen::getID(usedIDs);

	shareSettlementAllocationEntries.push_back(entry);

	qInfo() << entry;

	return entry.shareSettlementAllocationEntryID;
}

std::vector<entry::ShareSettlementAllocation> ShareSettlementRepoInMem::getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const
{
	std::vector<entry::ShareSettlementAllocation> filteredEntries{};

	for (const auto& allocEntry : shareSettlementAllocationEntries)
	{
		if (allocEntry.debtEntryID == debtEntryID)
			filteredEntries.push_back(allocEntry);
	}

	return filteredEntries;
}

double ShareSettlementRepoInMem::getTotalAllocatedShareSettlements(const QDate& minDate) const 
{
	double total{};

	for (const auto& entry : shareSettlementEntries)
	{
		if (getShareSettlementEntry(entry.shareSettlementEntryID).date < minDate) continue;
		total += entry.amount;
	}

	return total;
}

entry::ShareSettlement ShareSettlementRepoInMem::getShareSettlementEntry(int64_t shareSettlementEntryID) const
{
	for (const auto& entry : shareSettlementEntries)
		if (entry.shareSettlementEntryID == shareSettlementEntryID) return entry;
}
