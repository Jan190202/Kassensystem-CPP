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

	virtual double getPersonsCredit(int64_t personEntryID) const override;
	virtual double getTotalDepositedCredit(const QDate& minDate) const override;
private:
	std::vector<entry::Credit> entries{};
};