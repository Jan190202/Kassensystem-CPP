#pragma once

#include "domain/repoInterface/BalanceRepository.h"
#include "domain/model/Entities.h"
#include <vector>

class BalanceRepoInMem : public BalanceRepository
{
public:
	BalanceRepoInMem() = default;
	virtual ~BalanceRepoInMem() = default;

	virtual int64_t addEntry(entry::Balance) override;
	virtual double getTotalEarnings() const override;
	virtual double getTotalSpendings() const override;
private:
	std::vector<entry::Balance> entries{};
};