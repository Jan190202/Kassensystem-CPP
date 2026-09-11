#pragma once
#include "domain/model/Entities.h"
#include <QDate>

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;
	virtual int64_t addCreditEntry(entry::Credit entry) = 0; // add credit entry to database, return associated ID

	virtual double getPersonsCredit(int64_t personID) const = 0; // summed credits of every entry associated with person
	virtual double getTotalDepositedCredit() const = 0; // summed credits of every entry in database
};