#pragma once

#include "domain/model/Entities.h"

class BalanceRepository
{
public:
	BalanceRepository() = default;
	virtual ~BalanceRepository() = default;

	virtual int64_t addEntry(const BalanceEntry&) = 0;
	virtual double getTotalEarnings() const = 0;
	virtual double getTotalSpendings() const = 0;
};