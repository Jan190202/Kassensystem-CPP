#pragma once
#include "domain/repointerface/ShareSettlementRepository.h"
#include <vector>

class SqliteShareSettlementRepository : public ShareSettlementRepository
{
public:
	SqliteShareSettlementRepository() = default;
	virtual ~SqliteShareSettlementRepository() = default;
	virtual int64_t addShareSettlementEntry(const entry::ShareSettlement& entrty) override;
	virtual int64_t addShareSettlementAllocationEntry(const entry::ShareSettlementAllocation& entrty) override;

	virtual std::vector<entry::ShareSettlementAllocation> getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const override;
	virtual double getTotalAllocatedShareSettlements(const QDate& minDate) const override;
};