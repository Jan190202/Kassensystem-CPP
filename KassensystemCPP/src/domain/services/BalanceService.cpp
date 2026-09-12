#include "BalanceService.h"
#include <optional>

BalanceService::BalanceService(const RepositoryBundle& repoBundle, const registerFinancials::State& stateBefore)
	: balanceRepo(repoBundle.balanceRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo), personRepo(repoBundle.personRepo), shareSettlementRepo(repoBundle.shareSettlementRepo), paymentRepo(repoBundle.paymentRepo), stateBefore(stateBefore) {}

int64_t BalanceService::addBalanceItem(const request::Balance& request)
{
	int64_t personEntryID{};
	if (request.coveringpersonEntryID.has_value())
	{
		personEntryID = request.coveringpersonEntryID.value();
	}
	else
	{
		personEntryID = -1;
	}
	
	entry::Balance entry{
		.balanceEntryID = 0,
		.type = request.type,
		.description = request.description,
		.amount = request.amount,
		.dateBooked = request.date,
		.dateAdded = QDate::currentDate(),
		.comment = request.comment,
		.personEntryID = personEntryID
	};

	if (request.coveringpersonEntryID.has_value() && entry.type == BalanceType::Spending)
	{
		addCredit(entry.personEntryID, entry.amount, entry.dateBooked, "Abteilungsausgabe übernommen");
	}
	
	return balanceRepo->addBalanceEntry(entry);
}

int64_t BalanceService::addCredit(int64_t personEntryID, double amount, const QDate& date, const std::string& description)
{
	// potential validity check here
	
	return creditRepo->addCreditEntry(
		entry::Credit{ 
			.creditEntryID = 0, 
			.personEntryID = personEntryID, 
			.date = date, 
			.amount = amount, 
			.description = description 
		});
}

std::vector<entry::Balance> BalanceService::getBalanceEntries(BalanceType type, const QDate& minDate) const
{
	auto entries = balanceRepo->getBalanceEntries(type, minDate);

	if (type == BalanceType::EarningAndSupplement)
	{
		entries.push_back(
			entry::Balance{
				.type = BalanceType::Earning,
				.description = "Einnahmen durch Getränkeverkäufe",
				.amount = debtRepo->getTotalShare(FinancialShare::Own, minDate),
				.dateBooked = QDate::currentDate()
			});

		//entries.push_back(
		//	entry::Balance{
		//		.type = BalanceType::Earning,
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
	double consumptionOwnShare{};
	
	for (const auto& entry : getBalanceEntries(BalanceType::Earning, stateBefore.date)) departmentEarnings += entry.amount;
	for (const auto& entry : getBalanceEntries(BalanceType::Spending, stateBefore.date)) departmentSpendings += entry.amount;
	consumptionOwnShare = debtRepo->getTotalShare(FinancialShare::Own, stateBefore.date);

	double savingsDiff = departmentEarnings - departmentSpendings + consumptionOwnShare;


	// cashDiff = (departmentEarnings - departmentSpendings) + (paidDebt (which is totalShare) - settledValue (which is foreignShare)) + depositedCredit
	double paidDebt{};
	double settledValue{};
	double depositedCredit{};

	paidDebt = paymentRepo->getTotalAllocatedPayments(stateBefore.date);
	settledValue = shareSettlementRepo->getTotalAllocatedShareSettlements(stateBefore.date);
	depositedCredit = creditRepo->getTotalDepositedCredit(stateBefore.date);

	double cashDiff = departmentEarnings - departmentSpendings + paidDebt - settledValue + depositedCredit;

	qDebug() << "Creating financial report";
	qDebug() << "ConsumptionOwnShare: " << consumptionOwnShare;
	qDebug() << "DepartmentEarnings: " << departmentEarnings;
	qDebug() << "DepartmentSpendings: " << departmentSpendings;
	qDebug() << "PaidDebt: " << paidDebt;
	qDebug() << "SettledValue: " << settledValue;
	qDebug() << "DepositedCredit: " << depositedCredit;
	qDebug() << "";

	double currentForeignCash = debtRepo->getForeignDue();


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
		.totalSpendings = departmentSpendings
	};

	return report;
}

AddSettlementException BalanceService::addShareSettlement(request::ShareSettlement request)
{
	if (request.amount < 0) 
		return AddSettlementException::AmountNegative;
	if (request.amount < 1e-9) 
		return AddSettlementException::AmountZero;
	if (request.amount - debtRepo->getForeignDue() > 1e-9)
		return AddSettlementException::AmountGreaterThanTotalForeignShare;

	entry::ShareSettlement entry{
		.shareSettlementEntryID = 0,
		.date = QDate::currentDate(),
		.amount = request.amount,
	};

	int64_t settlementEntryID = shareSettlementRepo->addShareSettlementEntry(entry);

	double overpaymentAmount = addShareSettlementAllocation(settlementEntryID, entry.amount);

	if (overpaymentAmount > 1e-9)
	{
		// case currently caught with AddSettlementException::AmountGreaterThantTotalForeignShare
		// if settlement in advance (or up-rounding) intended later, implement here
	}

	debtRepo->getForeignShareOutstandingEntries(FilterType::OmitFullyPaid);

	return AddSettlementException::None;
}

double BalanceService::addShareSettlementAllocation(int64_t settlementEntryID, double amount)
{
	std::vector<entry::Outstanding> remainingDebtEntries = debtRepo->getForeignShareOutstandingEntries(FilterType::OmitFullyPaid);

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