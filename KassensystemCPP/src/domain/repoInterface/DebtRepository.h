#pragma once

#include "domain/model/Entities.h"
#include "domain/model/DomainTypes.h"

class DebtRepository
{
public:
	DebtRepository() = default;
	virtual ~DebtRepository() = default;

	virtual int64_t addEntry(entry::Debt) = 0;
	virtual double getTotal(int64_t personID) const = 0; // total consumption debt of person (paid and not paid)
	virtual double getTotal(FinancialShare share) const = 0; // total share for all saved consumptions (ows/foreign/all)
	virtual double getDue(int64_t personID) const = 0; // due amount a person still needs to pay for consumption
	virtual double getDue() const = 0; // due amount to settle for foreign share
	virtual double getPaid(int64_t personID) const = 0; // amount a person has already paid for consumption
	virtual std::vector<entry::Outstanding> getPaymentOutstandingEntries(int64_t personID, FilterType filter) const = 0;
	virtual std::vector<entry::Outstanding> getSettlementOutstandingEntries(FilterType filter) const = 0;
};