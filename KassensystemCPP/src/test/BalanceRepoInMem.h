#pragma once

#include "domain/repoInterface/BalanceRepository.h"
#include "entities/BalanceEntry.h"
#include <vector>

class BalanceRepoInMem : public BalanceRepository
{
public:
	BalanceRepoInMem() = default;
	virtual ~BalanceRepoInMem() = default;

	virtual void addBalanceEntry(const BalanceEntry&);
	virtual double getTotalEarnings() const;
	virtual double getTotalSpendings() const;
private:
	std::vector<BalanceEntry> entries{};
};