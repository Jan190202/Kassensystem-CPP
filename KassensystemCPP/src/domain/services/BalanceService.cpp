#include "BalanceService.h"
#include <QDateTime>
#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>

namespace
{
	constexpr double tolerance = 5e-4; // half of the display resolution (3 decimals)
}

BalanceService::BalanceService(const RepositoryBundle& repoBundle, PendingChangeLog& log)
	: balanceRepo(repoBundle.balanceRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo),
	personRepo(repoBundle.personRepo), shareSettlementRepo(repoBundle.shareSettlementRepo),
	paymentRepo(repoBundle.paymentRepo), financialReviewRepo(repoBundle.financialReviewRepo), log(log) {}

// writing

int64_t BalanceService::addBalanceItem(const request::Balance& request)
{
	entry::Balance entry{
		.balanceEntryID = 0,
		.type = request.type,
		.description = request.description,
		.amount = request.amount,
		.dateBooked = request.dateBooked,
		.dateAdded = QDate::currentDate(),
		.comment = request.comment,
		.person = request.coveringPerson
	};

	if (request.coveringPerson.has_value() && hasFlag(entry.type, BalanceType::spending))
	{
		addCredit(entry.person.value(), entry.amount, entry.dateBooked, "Abteilungsausgabe übernommen");
	}

	log.record(PendingChangeLog::ChangeType::add, entry);
	return balanceRepo->addBalanceEntry(entry);
}

int64_t BalanceService::addCredit(const entry::Person& person, double amount, const RegisterDate& date, const std::string& description)
{
	// potential validity check here

	auto entry = entry::Credit{
			.creditEntryID = 0,
			.person = person,
			.dateBooked = date,
			.dateAdded = QDate::currentDate(),
			.amount = amount,
			.description = description
	};

	log.record(PendingChangeLog::ChangeType::add, entry);
	return creditRepo->addCreditEntry(entry);
}

AddSettlementException BalanceService::addShareSettlement(request::ShareSettlement request)
{
	if (request.amount < 0)
		return AddSettlementException::amountNegative;
	if (request.amount < 1e-9)
		return AddSettlementException::amountZero;
	if (request.amount - debtRepo->getForeignDue() > 1e-9)
		return AddSettlementException::amountGreaterThanTotalForeignShare;

	entry::ShareSettlement entry{
		.shareSettlementEntryID = 0,
		.dateAdded = QDate::currentDate(),
		.amount = request.amount,
		.comment = request.comment
	};

	int64_t settlementEntryID = shareSettlementRepo->addShareSettlementEntry(entry);
	log.record(PendingChangeLog::ChangeType::add, entry);

	double overpaymentAmount = addShareSettlementAllocation(settlementEntryID, entry.amount);

	if (overpaymentAmount > 1e-9)
	{
		// case currently caught with AddSettlementException::amountGreaterThanTotalForeignShare
		// if settlement in advance (or up-rounding) intended later, implement here
	}

	return AddSettlementException::none;
}

double BalanceService::addShareSettlementAllocation(int64_t settlementEntryID, double amount)
{
	std::vector<entry::Outstanding> remainingDebtEntries = debtRepo->getForeignShareOutstandingEntries(FilterType::omitFullyPaid);

	double amountLeft = amount;
	for (const auto& entryRem : remainingDebtEntries)
	{
		if (amountLeft < 1e-9) break;

		double appliedToCurrentEntry = std::min(amountLeft, entryRem.remaining);

		amountLeft -= appliedToCurrentEntry;

		entry::ShareSettlementAllocation aEntry{
			.shareSettlementAllocationEntryID = 0,
			.debtEntryID = entryRem.debtEntryID,
			.shareSettlementEntryID = settlementEntryID,
			.amount = appliedToCurrentEntry
		};

		shareSettlementRepo->addShareSettlementAllocationEntry(aEntry);
	}

	return amountLeft;
}

int64_t BalanceService::addFinancialReview(const request::FinancialReview& request)
{
	const size_t count = getPeriodCount();
	const double expectedCash = count == 0 ? 0.0 : getPeriodReport(count - 1).end.cash;

	entry::FinancialReview entry{
		.financialReviewEntryID = 0,
		.dateTime = QDateTime::currentDateTime(),
		.countedCash = request.countedCash,
		.expectedCash = expectedCash,
		.comment = request.comment
	};

	int64_t id = financialReviewRepo->addFinancialReviewEntry(entry);
	log.record(PendingChangeLog::ChangeType::add, entry);
	return id;
}

int64_t BalanceService::addOpeningReview(const QDate& date, double countedCash)
{
	if (!loadReviews().empty())
		return -1;

	// "count = end of that day": everything booked on `date` belongs to the time before this review
	entry::FinancialReview entry{
		.financialReviewEntryID = 0,
		.dateTime = QDateTime(date, QTime(23, 59, 59)),
		.countedCash = countedCash,
		.expectedCash = countedCash,
		.comment = "Anfangsbestand (aus Konfiguration übernommen)"
	};

	int64_t id = financialReviewRepo->addFinancialReviewEntry(entry);
	log.record(PendingChangeLog::ChangeType::add, entry);
	return id;
}

// reading

std::vector<entry::Balance> BalanceService::getBalanceEntries(BalanceType type, const QDate& afterExclusive,
	const std::optional<QDate>& untilInclusive) const
{
	// journal entries only; consumption is reported separately (ConsumptionSummary)
	return balanceRepo->getBalanceEntries(type, DateRange{ afterExclusive, untilInclusive });
}

size_t BalanceService::getPeriodCount() const
{
	return loadReviews().size();
}

registerFinancials::PeriodReport BalanceService::getPeriodReport(size_t periodIndex) const
{
	const auto reviews = loadReviews();
	if (periodIndex >= reviews.size())
		throw std::out_of_range("BalanceService::getPeriodReport: period index out of range");

	const auto& startReview = reviews.at(periodIndex);
	const bool isCurrent = periodIndex + 1 == reviews.size();

	const QDate startDate = startReview.dateTime.date();
	std::optional<QDate> endDate; // nullopt = open end (current period)
	if (!isCurrent)
		endDate = reviews.at(periodIndex + 1).dateTime.date();

	const DateRange range{ startDate, endDate };

	// cash side: counted cash at start + cash events = expected cash at end
	const Flows flows = computeFlows(range);
	const registerFinancials::State start = buildState(startDate, startReview.countedCash, startDate);
	const double expectedCashEnd = start.cash + flows.cashChange();

	registerFinancials::State end;
	double cashCorrection = 0.0;
	bool changedSinceReview = false;

	if (isCurrent)
	{
		end = buildState(QDate::currentDate(), expectedCashEnd, std::nullopt);
	}
	else
	{
		const auto& endReview = reviews.at(periodIndex + 1);
		end = buildState(*endDate, endReview.countedCash, endDate);

		// difference is recomputed so that all identities in the report hold for the current data;
		// the value stored at review time only serves to detect later changes
		cashCorrection = endReview.countedCash - expectedCashEnd;
		changedSinceReview = std::abs(expectedCashEnd - endReview.expectedCash) > tolerance;
	}

	// assemble
	registerFinancials::PeriodReport report;
	report.start = start;
	report.end = end;
	report.periodStart = startDate;
	report.periodEnd = endDate;
	report.isCurrent = isCurrent;
	report.changedSinceReview = changedSinceReview;
	report.totalEarnings = flows.earnings;
	report.totalSpendings = flows.spendings;

	report.details.departmentEarnings = flows.earnings; // includes tips from overpayment
	report.details.departmentSpendings = flows.spendings;
	report.details.paidDebt = flows.paidDebt;
	report.details.settledValue = flows.settled;
	report.details.depositedCredit = flows.depositedCredit;
	report.details.cashCorrection = cashCorrection;

	report.consumption = buildConsumption(range, flows, start, end);

	return report;
}

//  helpers

std::vector<entry::FinancialReview> BalanceService::loadReviews() const
{
	auto reviews = financialReviewRepo->getAllFinancialReviewEntries();

	std::sort(reviews.begin(), reviews.end(), [](const entry::FinancialReview& a, const entry::FinancialReview& b)
		{
			return a.dateTime < b.dateTime;
		});

	return reviews;
}

double BalanceService::sumBalance(BalanceType type, const DateRange& range) const
{
	double sum = 0.0;
	for (const auto& entry : balanceRepo->getBalanceEntries(type, range))
		sum += entry.amount;
	return sum;
}

BalanceService::Flows BalanceService::computeFlows(const DateRange& range) const
{
	Flows flows;
	flows.earnings = sumBalance(BalanceType::earning, range);
	flows.spendings = sumBalance(BalanceType::spending, range);
	flows.paidDebt = paymentRepo->getTotalAllocatedPayments(range);
	flows.settled = shareSettlementRepo->getTotalAllocatedShareSettlements(range);
	flows.depositedCredit = creditRepo->getTotalDepositedCredit(range);
	return flows;
}

// Everything except `cash` is derived from the database as of `asOf` (nullopt = now).
// The three stocks are read with dedicated "outstanding" queries (per-entry remainders), independent of the
// flow sums above, so the roll-forward in the report is a real cross-check and not a tautology.
registerFinancials::State BalanceService::buildState(const QDate& date, double cash, const std::optional<QDate>& asOf) const
{
	const DateRange upTo = DateRange::upTo(asOf);

	registerFinancials::State state;
	state.date = date;
	state.cash = cash;
	state.foreignCash = debtRepo->getForeignDue(upTo);
	state.debt = debtRepo->getOpenDebt(upTo);
	state.credit = creditRepo->getOpenCredit(upTo);
	state.ownCash = cash - state.foreignCash;
	state.savings = cash - state.foreignCash + state.debt - state.credit;
	return state;
}

registerFinancials::ConsumptionSummary BalanceService::buildConsumption(const DateRange& range, const Flows& flows,
	const registerFinancials::State& start,
	const registerFinancials::State& end) const
{
	const double allShares = debtRepo->getTotalShare(FinancialShare::all, range);
	const double ownShare = debtRepo->getTotalShare(FinancialShare::own, range);
	const double foreignShare = allShares - ownShare;

	registerFinancials::ConsumptionSummary c;
	c.totalConsumption = allShares;
	c.departmentShare = ownShare;

	// begin + added - removed = end; endDirect comes from the independent stock queries in buildState
	c.debt = { start.debt, allShares, flows.paidDebt, end.debt };
	c.foreignShare = { start.foreignCash, foreignShare, flows.settled, end.foreignCash };
	c.credit = { start.credit, flows.depositedCredit, creditRepo->getTotalUsedCredit(range), end.credit };

	return c;
}