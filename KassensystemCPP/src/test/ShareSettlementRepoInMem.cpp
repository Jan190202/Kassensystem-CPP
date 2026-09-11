#include "ShareSettlementRepoInMem.h"
#include "IDGenerator.h"

int64_t ShareSettlementRepoInMem::addShareSettlementEntry(entry::ShareSettlement entry)
{
	std::vector<int64_t> usedIDs(shareSettlementEntries.size());
	for (size_t i = 0; i < shareSettlementEntries.size(); i++) 
		usedIDs.at(i) = shareSettlementEntries.at(i).shareSettlementID;
	entry.shareSettlementID = idgen::getID(usedIDs);
	
	shareSettlementEntries.push_back(entry);

	qInfo() << entry;

	return entry.shareSettlementID;
}

int64_t ShareSettlementRepoInMem::addShareSettlementAllocationEntry(entry::ShareSettlementAllocation entry)
{
	std::vector<int64_t> usedIDs(shareSettlementAllocationEntries.size());
	for (size_t i = 0; i < shareSettlementAllocationEntries.size(); i++) 
		usedIDs.at(i) = shareSettlementAllocationEntries.at(i).shareSettlementID;
	entry.shareSettlementAllocationID = idgen::getID(usedIDs);

	shareSettlementAllocationEntries.push_back(entry);

	qInfo() << entry;

	return entry.shareSettlementAllocationID;
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

double ShareSettlementRepoInMem::getTotalAllocatedShareSettlements() const
{
	double total{};

	for (const auto& entry : shareSettlementEntries)
		total += entry.amount;

	return total;
}