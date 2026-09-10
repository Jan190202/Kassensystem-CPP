#pragma once
#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"

class DebtRepository
{
public:
	DebtRepository() = default;
	virtual ~DebtRepository() = default;
	virtual int64_t addDebtEntry(entry::Debt entry) = 0; // add debt entry to database, return associated ID

	virtual double getPersonsTotal(int64_t personID) const = 0; // total consumption debt of person (paid and not paid)
	virtual double getPersonsDue(int64_t personID) const = 0; // due amount a person still needs to pay for consumption
	virtual double getPersonsPaid(int64_t personID) const = 0; // amount a person has already paid for consumption
	virtual double getTotalShare(FinancialShare share) const = 0; // total share for all saved consumptions (ows/foreign/all)
	virtual double getForeignDue() const = 0; // due amount to settle for foreign share
	virtual std::vector<entry::Outstanding> getPersonsOutstandingEntries(int64_t personID, FilterType filter) const = 0; // return outstanding entries for persons debt
	virtual std::vector<entry::Outstanding> getForeignShareOutstandingEntries(FilterType filter) const = 0; // return outstanding entries for foreign share settlement
};