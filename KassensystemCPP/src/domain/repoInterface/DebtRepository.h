#pragma once

#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"

class DebtRepository
{
public:
	DebtRepository() = default;
	virtual ~DebtRepository() = default;

	virtual int64_t addEntry(entry::Debt) = 0;
	virtual double getTotal(int64_t personID) const = 0;
	virtual double getTotal(FinancialShare share) const = 0;
	virtual double getDue(int64_t personID) const = 0;
	virtual double getSettled(int64_t personID) const = 0;
	virtual std::vector<entry::Outstanding> getPaymentOutstandingEntries(int64_t personID, FilterType filter) const = 0;
	virtual std::vector<entry::Outstanding> getSettlementOutstandingEntries(FilterType filter) const = 0;
};