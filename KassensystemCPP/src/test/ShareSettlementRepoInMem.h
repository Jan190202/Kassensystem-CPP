#pragma once
#include "domain/repointerface/ShareSettlementRepository.h"

class ShareSettlementRepoInMem : public ShareSettlementRepository
{
public:
	ShareSettlementRepoInMem() = default;
	virtual ~ShareSettlementRepoInMem() = default;
	virtual int64_t addShareSettlementEntry(entry::ShareSettlement entry) override;
	virtual int64_t addShareSettlementAllocationEntry(entry::ShareSettlementAllocation entry) override;

	virtual std::vector<entry::ShareSettlementAllocation> getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const override;
	virtual double getTotalAllocatedShareSettlements(const QDate& minDate) const override;

private:
	entry::ShareSettlement getShareSettlementEntry(int64_t shareSettlementEntryID) const;

	std::vector<entry::ShareSettlement> shareSettlementEntries{};
	std::vector<entry::ShareSettlementAllocation> shareSettlementAllocationEntries{};
};