#pragma once

#include "domain/repoInterface/CreditRepository.h"
#include <QDate>
#include <vector>

class CreditRepoInMem : public CreditRepository
{
public:
	CreditRepoInMem() = default;
	virtual ~CreditRepoInMem() = default;

	virtual int64_t addEntry(entry::Credit) override;
	virtual void resetCredit(int64_t personID) override;
	virtual double getCredit(int64_t personID) const override;
private:
	std::vector<entry::Credit> entries{};
};