#pragma once

#include <QDate>
#include "domain/model/Entities.h"

class CreditRepository
{
public:
	CreditRepository() = default;
	virtual ~CreditRepository() = default;

	virtual int64_t addEntry(const entry::Credit&) = 0;
	virtual double getCredit(int64_t personID) const = 0;
};