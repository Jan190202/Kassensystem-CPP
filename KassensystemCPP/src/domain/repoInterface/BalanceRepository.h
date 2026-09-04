#pragma once

#include "domain/model/Entities.h"
#include <vector>

class BalanceRepository
{
public:
	BalanceRepository() = default;
	virtual ~BalanceRepository() = default;

	virtual int64_t addEntry(entry::Balance) = 0;
	virtual double getTotalEarnings() const = 0;
	virtual double getTotalSpendings() const = 0;
	virtual std::vector<entry::Balance> getEntries(BalanceType) const = 0;
};