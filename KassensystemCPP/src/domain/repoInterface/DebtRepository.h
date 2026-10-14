#pragma once
#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"
#include <vector>

class DebtRepository
{
public:
	DebtRepository() = default;
	virtual ~DebtRepository() = default;
	virtual int64_t addDebtEntry(const entry::Debt& entry) = 0; // add debt entry to database, return associated ID

	virtual double getPersonsTotal(int64_t personEntryID) const = 0; // total consumption debt of person (paid and not paid)
	virtual double getPersonsDue(int64_t personEntryID) const = 0; // due amount a person still needs to pay for consumption
	virtual double getPersonsPaid(int64_t personEntryID) const = 0; // amount a person has already paid for consumption

	virtual double getTotalShare(FinancialShare share, const DateRange& range) const = 0; // total share (own/foreign/all) of all debt entries booked within range (DateRange::contains(dateBooked, dateAdded))

	virtual double getForeignDue() const = 0; // due amount to settle for foreign share right now
	virtual double getForeignDue(const DateRange& range) const = 0; // foreign share still unsettled as of untilInclusive: per debt entry (booked up to then) its foreign share minus settlement allocations whose settlement was added up to then
	virtual double getOpenDebt(const DateRange& range) const = 0; // consumption still unpaid as of untilInclusive: per debt entry (booked up to then) its amount minus payment allocations whose payment was added up to then

	virtual std::vector<entry::Outstanding> getPersonsOutstandingEntries(int64_t personEntryID, FilterType filter) const = 0; // return outstanding entries for persons debt
	virtual std::vector<entry::Outstanding> getForeignShareOutstandingEntries(FilterType filter) const = 0; // return outstanding entries for foreign share settlement
};