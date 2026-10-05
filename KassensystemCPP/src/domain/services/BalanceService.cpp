#include "BalanceService.h"
#include <optional>

BalanceService::BalanceService(const RepositoryBundle& repoBundle, const registerFinancials::State& stateBefore, PendingChangeLog& log)
	: balanceRepo(repoBundle.balanceRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo), personRepo(repoBundle.personRepo), shareSettlementRepo(repoBundle.shareSettlementRepo), paymentRepo(repoBundle.paymentRepo), stateBefore(stateBefore), log(log) {}

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

std::vector<entry::Balance> BalanceService::getBalanceEntries(BalanceType type, const QDate& minDate) const
{
	auto entries = balanceRepo->getBalanceEntries(type, minDate);

	if (hasFlag(type, BalanceType::supplement))
	{
		entries.push_back(
			entry::Balance{
				.type = BalanceType::earning | BalanceType::supplement,
				.description = "Einnahmen durch Getränkeverkäufe",
				.amount = debtRepo->getTotalShare(FinancialShare::own, minDate),
				.dateBooked = QDate::currentDate()
			});

		//entries.push_back(
		//	entry::Balance{
		//		.type = BalanceType::earning | BalanceType::supplement,
		//		.description = "Rundungsfehler bei Abrechnung (kum.)",
		//		.amount = 0, //settlementRepo->getTotalRounding(),
		//		.dateBooked = QDate::currentDate()
		//	});
	}
	
	return entries;
}

registerFinancials::Report BalanceService::getReport() const
{
	
	// savingsDiff = (departmentEarnings - departmentSpendings) + virtual own consumption share
	double departmentEarnings{}; // includes tips from overpayment
	double departmentSpendings{};
	double consumptionAllShares{};
	double consumptionOwnShare{};
	double consumptionForeignShare{};
	
	for (const auto& entry : getBalanceEntries(BalanceType::earning, stateBefore.date)) departmentEarnings += entry.amount;
	for (const auto& entry : getBalanceEntries(BalanceType::spending, stateBefore.date)) departmentSpendings += entry.amount;
	consumptionAllShares = debtRepo->getTotalShare(FinancialShare::all, stateBefore.date);
	consumptionOwnShare = debtRepo->getTotalShare(FinancialShare::own, stateBefore.date);
	consumptionForeignShare = consumptionAllShares - consumptionOwnShare;

	double savingsDiff = departmentEarnings - departmentSpendings + consumptionOwnShare;


	// cashDiff = (departmentEarnings - departmentSpendings) + (paidDebt (which is totalShare) - settledValue (which is foreignShare)) + depositedCredit
	double paidDebt{};
	double settledValue{};
	double depositedCredit{};

	paidDebt = paymentRepo->getTotalAllocatedPayments(stateBefore.date);
	settledValue = shareSettlementRepo->getTotalAllocatedShareSettlements(stateBefore.date);
	depositedCredit = creditRepo->getTotalDepositedCredit(stateBefore.date);

	double cashDiff = departmentEarnings - departmentSpendings + paidDebt - settledValue + depositedCredit;
	double currentForeignCash = debtRepo->getForeignDue();

	// all-time values for correctness check
	double consumptionAllSharesAllTime = debtRepo->getTotalShare(FinancialShare::all, QDate(2000,1,1));
	double consumptionOwnShareAllTime = debtRepo->getTotalShare(FinancialShare::own, QDate(2000, 1, 1));
	double consumptionForeignShareAllTime = consumptionAllSharesAllTime - consumptionOwnShareAllTime;
	double paidDebtAllTime = paymentRepo->getTotalAllocatedPayments(QDate(2000, 1, 1));
	double depositedCreditAllTime = creditRepo->getTotalDepositedCredit(QDate(2000, 1, 1));


	// struct construction
	registerFinancials::State stateAfter{
	.date = QDate::currentDate(),
	.cash = stateBefore.cash + cashDiff,
	.savings = stateBefore.savings + savingsDiff,
	.ownCash = stateBefore.cash + cashDiff - currentForeignCash,
	.foreignCash = currentForeignCash
	};

	registerFinancials::Report report{
		.stateBefore = stateBefore,
		.stateAfter = stateAfter,
		.savingsDiff = savingsDiff,
		.cashDiff = cashDiff,
		.totalEarnings = departmentEarnings + consumptionOwnShare,
		.totalSpendings = departmentSpendings,
		.details = registerFinancials::Report::Details{
			.departmentEarnings = departmentEarnings,
			.departmentSpendings = departmentSpendings,
			.consumptionAllShares = consumptionAllShares,
			.consumptionOwnShare = consumptionOwnShare,
			.consumptionForeignShare = consumptionForeignShare,
			.paidDebt = paidDebt,
			.settledValue = settledValue,
			.depositedCredit = depositedCredit,
			.consumptionAllSharesAllTime = consumptionAllSharesAllTime,
			.consumptionOwnShareAllTime = consumptionOwnShareAllTime,
			.consumptionForeignShareAllTime = consumptionForeignShareAllTime,
			.paidDebtAllTime = paidDebtAllTime,
			.depositedCreditAllTime = depositedCreditAllTime
		}	
	};

	return report;
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
		// case currently caught with AddSettlementException::AmountGreaterThantTotalForeignShare
		// if settlement in advance (or up-rounding) intended later, implement here
	}

	debtRepo->getForeignShareOutstandingEntries(FilterType::omitFullyPaid);

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