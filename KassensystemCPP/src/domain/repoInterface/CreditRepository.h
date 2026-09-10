#pragma once
#include "domain/model/Entities.h"
#include <QDate>

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;
	virtual int64_t addCreditEntry(entry::Credit entry) = 0; // add credit entry to database, return associated ID
	virtual void resetPersonsCredit(int64_t personID) = 0; // delete all credit entries associated with person, TBD: replace calls with addEntres and negative credit

	virtual double getPersonsCredit(int64_t personID) const = 0; // summed credits of every entry associated with person
	virtual double getTotalDepositedCredit() const = 0; // summed credits of every entry in database
};