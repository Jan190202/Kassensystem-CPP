#include "SettlementRepoInMem.h"
#include "IDGenerator.h"

int64_t SettlementRepoInMem::addSettlementEntry(entry::Settlement entry)
{
	std::vector<int64_t> usedIDs(settlementEntries.size());
	for (size_t i = 0; i < settlementEntries.size(); i++) 
		usedIDs.at(i) = settlementEntries.at(i).settlementID;
	entry.settlementID = idgen::getID(usedIDs);
	
	settlementEntries.push_back(entry);
	return entry.settlementID;
}

int64_t SettlementRepoInMem::addSettlementAllocationEntry(entry::SettlementAllocation entry)
{
	std::vector<int64_t> usedIDs(settlementAllocationEntries.size());
	for (size_t i = 0; i < settlementAllocationEntries.size(); i++) 
		usedIDs.at(i) = settlementAllocationEntries.at(i).settlementID;
	entry.settlementAllocationID = idgen::getID(usedIDs);

	settlementAllocationEntries.push_back(entry);
	return entry.settlementAllocationID;
}