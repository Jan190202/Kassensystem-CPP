#pragma once

#include "domain/repointerface/BalanceRepository.h"
#include "domain/repointerface/CreditRepository.h"
#include "domain/repointerface/DebtRepository.h"
#include "domain/repointerface/PersonRepository.h"
#include "domain/repointerface/SettlementRepository.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"

#include <vector>

#include <QDate>

enum class AddSettlementException
{
	None, AmountNegative, AmountZero, AmountGreaterThanTotalForeignShare
};

class BalanceService
{
public:
	BalanceService(BalanceRepository* balanceRepo, CreditRepository* creditRepo, DebtRepository* debtRepo, PersonRepository* personRepo, SettlementRepository* settlementRepo, const registerFinancials::State& stateBefore);

	int64_t addEntry(const request::Balance&);
	std::vector<entry::Balance> getEntries(BalanceType) const;

	registerFinancials::Report getReport() const;

	AddSettlementException addSettlement(request::Settlement request);
private:
	int64_t addCredit(int64_t personID, double amount, QDate date, std::string description);
	double addSettlementAllocation(int64_t settlementEntryID, double amount);

	BalanceRepository* balanceRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
	SettlementRepository* settlementRepo;
	const registerFinancials::State& stateBefore;
};