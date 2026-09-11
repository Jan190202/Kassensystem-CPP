#pragma once
#include "app/RepositoryBundle.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include <QDate>
#include <vector>

enum class AddSettlementException
{
	None, AmountNegative, AmountZero, AmountGreaterThanTotalForeignShare
};

class BalanceService
{
public:
	inline static const QDate earliestDate = QDate{ 2000,1,1 };

	BalanceService(const RepositoryBundle& repoBundle, const registerFinancials::State& stateBefore);
	int64_t addBalanceItem(const request::Balance& request);
	AddSettlementException addShareSettlement(request::ShareSettlement request);

	std::vector<entry::Balance> getBalanceEntries(BalanceType type, const QDate& minDate = earliestDate) const;
	registerFinancials::Report getReport() const;
		
private:
	int64_t addCredit(int64_t personEntryID, double amount, const QDate& date, const std::string& description);
	double addShareSettlementAllocation(int64_t settlementEntryID, double amount);

	BalanceRepository* balanceRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
	ShareSettlementRepository* shareSettlementRepo;
	PaymentRepository* paymentRepo;
	const registerFinancials::State& stateBefore;
};