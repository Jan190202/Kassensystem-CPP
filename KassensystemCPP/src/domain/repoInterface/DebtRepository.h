#pragma once

#include "domain/model/Entities.h"

class DebtRepository
{
public:
	DebtRepository() = default;
	virtual ~DebtRepository() = default;

	virtual int64_t addEntry(const DebtEntry&) = 0;
	virtual double getTotal(int64_t personID) const = 0;
	virtual std::vector<DebtEntryRemaining> getOutstandingEntries(int64_t personID) const = 0;
};