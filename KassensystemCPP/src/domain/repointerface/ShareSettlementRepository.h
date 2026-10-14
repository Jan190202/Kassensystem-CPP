#pragma once
#include "domain/model/Entities.h"
#include <QDate>
#include <vector>
#include <string>

class ShareSettlementRepository
{
public:
	ShareSettlementRepository() = default;
	virtual ~ShareSettlementRepository() = default;
	virtual int64_t addShareSettlementEntry(const entry::ShareSettlement& entry) = 0; // add settlement entry, return associated ID
	virtual int64_t addShareSettlementAllocationEntry(const entry::ShareSettlementAllocation& entry) = 0; // add settlement allocation entry, return associated ID

	virtual std::vector<entry::ShareSettlementAllocation> getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const = 0; // foreign share settlement allocation entries associated with debt entry
	virtual double getTotalAllocatedShareSettlements(const DateRange& range) const = 0; // summed amounts of all settlement allocations whose settlement was added within range (filters on the settlement's dateAdded: that is when the cash moved)
};