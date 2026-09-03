#pragma once

#include "domain/repoInterface/BalanceRepository.h"
#include "domain/model/Entities.h"
#include <vector>

class BalanceRepoInMem : public BalanceRepository
{
public:
	BalanceRepoInMem() = default;
	virtual ~BalanceRepoInMem() = default;

	virtual int64_t addEntry(const entry::Balance&);
	virtual double getTotalEarnings() const;
	virtual double getTotalSpendings() const;
private:
	std::vector<entry::Balance> entries{};
};