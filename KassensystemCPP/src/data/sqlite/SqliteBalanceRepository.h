#pragma once
#include "domain/repointerface/BalanceRepository.h"

class SqliteBalanceRepository : public BalanceRepository
{
public:
	SqliteBalanceRepository() = default;
	virtual ~SqliteBalanceRepository() = default;
	virtual int64_t addBalanceEntry(entry::Balance) override;

	virtual std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> getBalanceEntry(const std::string& description) const override;
	virtual std::vector<entry::Balance> getBalanceEntries(BalanceType type, const QDate& minDate) const override;
};