#pragma once
#include "domain/model/Entities.h"
#include <QDate>

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;
	virtual int64_t addCreditEntry(const entry::Credit& entry) = 0; // add credit entry to database, return associated ID

	virtual double getPersonsCredit(int64_t personEntryID) const = 0; // summed credits of every entry associated with person

	virtual double getTotalDepositedCredit(const DateRange& range) const = 0; // summed credits of all entries booked within range (DateRange::contains(dateBooked, dateAdded))
	virtual double getTotalUsedCredit(const DateRange& range) const = 0; // credit consumed (offset against debts) within range, by the day the using entry was added; 0 if the model never offsets credit
	virtual double getOpenCredit(const DateRange& range) const = 0; // deposited minus used credit as of untilInclusive
};