#pragma once

#include "domain/repoInterface/SettlementRepository.h"

class SettlementRepoInMem : public SettlementRepository
{
public:
	SettlementRepoInMem() = default;
	virtual ~SettlementRepoInMem() = default;

	virtual int64_t addSettlementEntry(entry::Settlement) override;
	virtual int64_t addSettlementAllocationEntry(entry::SettlementAllocation) override;

	virtual std::vector<entry::SettlementAllocation> getAllocEntries(int64_t debtEntryID) const override;
private:
	std::vector<entry::Settlement> settlementEntries;
	std::vector<entry::SettlementAllocation> settlementAllocationEntries;
};