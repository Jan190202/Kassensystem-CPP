#pragma once

#include "domain/repoInterface/DebtRepository.h"
#include <vector>

class DebtRepoInMem : public DebtRepository
{
public:
	DebtRepoInMem() = default;
	virtual ~DebtRepoInMem() = default;

	virtual int64_t addEntry(const DebtEntry& entry) override;
	virtual double getTotal(int64_t personID) const override;
	virtual std::vector<DebtEntryRemaining> getOutstandingEntries(int64_t personID) const override;
private:
	std::vector<DebtEntry> entries;
};