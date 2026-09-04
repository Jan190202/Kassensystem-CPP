#pragma once

#include "domain/repoInterface/BalanceRepository.h"
#include "domain/repoInterface/CreditRepository.h"
#include "domain/repoInterface/PersonRepository.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"

#include <vector>

#include <QDate>

class BalanceService
{
public:
	BalanceService(BalanceRepository* balanceRepo, CreditRepository* creditRepo, PersonRepository* personRepo);

	int64_t addEntry(const BalanceRequest&);
	double getTotalEarnings() const;
	double getTotalSpendings() const;
	std::vector<entry::Balance> getEntries(BalanceType) const;
private:
	int64_t addCredit(int64_t personID, double amount, QDate date, std::string description);

	BalanceRepository* balanceRepo;
	CreditRepository* creditRepo;
	PersonRepository* personRepo;
};