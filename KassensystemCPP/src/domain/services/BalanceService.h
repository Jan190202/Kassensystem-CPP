#pragma once

#include "app/RepositoryBundle.h"
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
	BalanceService(const RepositoryBundle& repoBundle, const registerFinancials::State& stateBefore);

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
	PaymentRepository* paymentRepo;
	const registerFinancials::State& stateBefore;
};