#pragma once
#include "domain/model/Entities.h"
#include <vector>
#include <string>

class ShareSettlementRepository
{
public:
	ShareSettlementRepository() = default;
	virtual ~ShareSettlementRepository() = default;
	virtual int64_t addShareSettlementEntry(entry::ShareSettlement entry) = 0; // add settlement entry, return associated ID
	virtual int64_t addShareSettlementAllocationEntry(entry::ShareSettlementAllocation entry) = 0; // add settlement allocation entry, return associated ID

	virtual std::vector<entry::ShareSettlementAllocation> getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const = 0; // foreign share settlement allocation entries associated with debt entry
	virtual double getTotalAllocatedShareSettlements() const = 0; // summed settlement amounts of every settlement allocation in the database
};