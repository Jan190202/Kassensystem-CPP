#pragma once

#include "domain/repoInterface/CreditRepository.h"
#include <QDate>
#include <vector>

struct CreditRepoRow
{
	Person person;
	double amount;
	QDate date = QDate::currentDate();
};

class CreditRepoInMem : public CreditRepository
{
public:
	CreditRepoInMem() = default;
	virtual ~CreditRepoInMem() = default;

	virtual void addCredit(const Person& person, double amount, QDate date = QDate::currentDate()) override;
	virtual double getCredit(const Person& person) const override;
private:
	std::vector<CreditRepoRow> entries = {};
};