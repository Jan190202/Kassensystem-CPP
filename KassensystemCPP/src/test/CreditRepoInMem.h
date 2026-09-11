#pragma once
#include "domain/repointerface/CreditRepository.h"
#include <QDate>
#include <vector>

class CreditRepoInMem : public CreditRepository
{
public:
	CreditRepoInMem() = default;
	virtual ~CreditRepoInMem() = default;

	virtual int64_t addCreditEntry(entry::Credit) override;
	virtual void resetPersonsCredit(int64_t personID) override;
	virtual double getPersonsCredit(int64_t personID) const override;
	virtual double getTotalDepositedCredit() const override;
private:
	std::vector<entry::Credit> entries{};
};