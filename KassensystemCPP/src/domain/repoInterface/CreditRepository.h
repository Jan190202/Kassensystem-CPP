#pragma once

#include <QDate>
#include "domain/model/Entities.h"

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;

	virtual int64_t addEntry(entry::Credit) = 0;
	virtual double getCredit(int64_t personID) const = 0;
	virtual void resetCredit(int64_t personID) = 0;
};