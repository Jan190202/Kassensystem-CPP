#pragma once
#include "domain/repointerface/BalanceRepository.h"
#include <QSqlQuery>

class SqliteBalanceRepository : public BalanceRepository
{
public:
	SqliteBalanceRepository() = default;
	virtual ~SqliteBalanceRepository() = default;
	virtual int64_t addBalanceEntry(const entry::Balance& entry) override;

	virtual std::expected<entry::Balance, GetEntryException> getBalanceEntry(const std::string& description) const override;
	virtual std::vector<entry::Balance> getBalanceEntries(BalanceType type, const QDate& minDate) const override;

private:
	entry::Balance getEntryFromQuery(const QSqlQuery& query) const;
};