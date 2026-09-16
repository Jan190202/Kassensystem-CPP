#pragma once
#include "domain/repointerface/CreditRepository.h"

class SqliteCreditRepository : public CreditRepository
{
public:
	SqliteCreditRepository() = default;
	virtual ~SqliteCreditRepository() = default;
	virtual int64_t addCreditEntry(const entry::Credit& entry) override;

	virtual double getPersonsCredit(int64_t personEntryID) const override;
	virtual double getTotalDepositedCredit(const QDate& minDate) const override;
};