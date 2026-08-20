#pragma once

#include "domain/repoInterface/CreditRepository.h"
#include <QDate>
#include <vector>

struct CreditRepoRow
{
	int personID;
	double amount;
	QDate date = QDate::currentDate();
};

class CreditRepoInMem : public CreditRepository
{
public:
	CreditRepoInMem() = default;
	virtual ~CreditRepoInMem() = default;

	virtual void addCredit(int personID, double amount, QDate date = QDate::currentDate()) override;
	virtual double getCredit(int personID) const override;
private:
	std::vector<CreditRepoRow> entries = {};
};