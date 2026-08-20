#pragma once

#include "entities/BalanceEntry.h"

class BalanceRepository
{
public:
	BalanceRepository() = default;
	virtual ~BalanceRepository() = default;

	virtual void addBalanceEntry(const BalanceEntry&) = 0;
	virtual double getTotalEarnings() const = 0;
	virtual double getTotalSpendings() const = 0;
};