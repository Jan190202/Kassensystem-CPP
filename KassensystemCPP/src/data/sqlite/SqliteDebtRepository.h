#pragma once
#include "domain/repointerface/DebtRepository.h"
#include <QSqlQuery>

class SqliteDebtRepository : public DebtRepository
{
public:
	SqliteDebtRepository() = default;
	virtual ~SqliteDebtRepository() = default;
	virtual int64_t addDebtEntry(const entry::Debt& entry) override;

	virtual double getPersonsTotal(int64_t personEntryID) const override;
	virtual double getPersonsDue(int64_t personEntryID) const override;
	virtual double getPersonsPaid(int64_t personEntryID) const override;
	virtual double getTotalShare(FinancialShare share, const QDate& minDate) const override;
	virtual double getForeignDue() const override;
	virtual std::vector<entry::Outstanding> getPersonsOutstandingEntries(int64_t personEntryID, FilterType filter) const override;
	virtual std::vector<entry::Outstanding> getForeignShareOutstandingEntries(FilterType filter) const override;
private:
	entry::Outstanding getOutstandingEntryFromQuery(const QSqlQuery& query) const;
};