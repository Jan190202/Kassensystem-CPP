#pragma once

#include "domain/repoInterface/DebtRepository.h"
#include <vector>

class DebtRepoInMem : public DebtRepository
{
public:
	DebtRepoInMem() = default;
	virtual ~DebtRepoInMem() = default;

	virtual int64_t addEntry(entry::Debt entry) override;
	virtual double getTotal(int64_t personID) const override;
	virtual double getDue(int64_t personID) const override;
	virtual double getSettled(int64_t personID) const override;
	virtual std::vector<entry::DebtRemaining> getOutstandingEntries(int64_t personID, FilterType filter) const override;
private:
	std::vector<entry::Debt> entries;
};