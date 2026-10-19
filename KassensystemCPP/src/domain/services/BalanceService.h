#pragma once
#include "app/RepositoryBundle.h"
#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include <QDate>
#include <optional>
#include <vector>

enum class AddSettlementException
{
	none, amountNegative, amountZero, amountGreaterThanTotalForeignShare
};

class BalanceService
{
public:
	BalanceService(const RepositoryBundle& repoBundle, PendingChangeLog& log);

	// writing
	int64_t addBalanceItem(const request::Balance& request);
	AddSettlementException addShareSettlement(const request::ShareSettlement& request);
	int64_t addFinancialReview(const request::FinancialReview& request);
	int64_t addOpeningReview(const QDate& date, double countedCash); // one-time import of the old config state; -1 if reviews already exist

	// reading
	std::vector<entry::Balance> getBalanceEntries(BalanceType type, const QDate& afterExclusive, const std::optional<QDate>& untilInclusive = std::nullopt) const;
	size_t getPeriodCount() const; // one period per review: review.at(i) -> review.at(i+1) (or now)
	registerFinancials::PeriodReport getPeriodReport(size_t periodIndex) const;

private:
	struct Flows // cash events of one period
	{
		double earnings = 0.0, spendings = 0.0;
		double paidDebt = 0.0, settled = 0.0, depositedCredit = 0.0;

		double cashChange() const { return earnings - spendings + paidDebt - settled + depositedCredit; }
	};

	int64_t addCredit(const entry::Person& person, double amount, const RegisterDate& date, const std::string& description);
	double addShareSettlementAllocation(int64_t settlementEntryID, double amount);

	std::vector<entry::FinancialReview> loadReviews() const; // ascending by dateTime
	double sumBalance(BalanceType type, const DateRange& range) const;
	Flows computeFlows(const DateRange& range) const;
	registerFinancials::State buildState(const QDate& date, double cash, const std::optional<QDate>& asOf) const;
	registerFinancials::ConsumptionSummary buildConsumption(const DateRange& range, const Flows& flows, const registerFinancials::State& start,	const registerFinancials::State& end) const;

	BalanceRepository* balanceRepo;
	CreditRepository* creditRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
	ShareSettlementRepository* shareSettlementRepo;
	PaymentRepository* paymentRepo;
	FinancialReviewRepository* financialReviewRepo;
	PendingChangeLog& log;
};