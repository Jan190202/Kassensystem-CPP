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
	virtual int64_t addBalanceEntry(entry::Balance) = 0; // add balance entry to database, return associated ID

	virtual std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> getBalanceEntry(const std::string& description) const = 0; // return balance entry associated with description
	virtual std::vector<entry::Balance> getBalanceEntries(BalanceType type, const QDate& minDate) const = 0; // return all balance entries of passed type
};