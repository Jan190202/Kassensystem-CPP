#pragma once

#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"
#include <vector>
#include <expected>
#include <functional>

class BalanceRepository
{
public:
	BalanceRepository() = default;
	virtual ~BalanceRepository() = default;

	virtual int64_t addEntry(entry::Balance) = 0;
	virtual std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException > getEntry(std::string description) const = 0;
	//virtual double getTotalEarnings() const = 0;
	//virtual double getTotalSpendings() const = 0;
	virtual std::vector<entry::Balance> getEntries(BalanceType) const = 0;
};