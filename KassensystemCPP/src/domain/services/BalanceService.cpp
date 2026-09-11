#include "BalanceService.h"
#include <optional>

BalanceService::BalanceService(const RepositoryBundle& repoBundle, const registerFinancials::State& stateBefore)
	: balanceRepo(repoBundle.balanceRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo), personRepo(repoBundle.personRepo), shareSettlementRepo(repoBundle.shareSettlementRepo), paymentRepo(repoBundle.paymentRepo), stateBefore(stateBefore) {}

int64_t BalanceService::addEntry(const request::Balance& request)
{
	int64_t personID{};
	if (request.coveringPersonID.has_value())
	{
		personID = request.coveringPersonID.value();
	}
	else
	{
		personID = -1;
	}
	
	entry::Balance entry{
		.balanceEntryID = 0,
		.type = request.type,
		.description = request.description,
		.amount = request.amount,
		.dateBooked = request.date,
		.dateAdded = QDate::currentDate(),
		.comment = request.comment,
		.personID = personID
	};

	if (request.coveringPersonID.has_value() && entry.type == BalanceType::Spending)
	{
		addCredit(entry.personID, entry.amount, entry.dateBooked, "Abteilungsausgabe übernommen");
	}
	
	return balanceRepo->addBalanceEntry(entry);
}

int64_t BalanceService::addCredit(int64_t personID, double amount, QDate date, const std::string& description)
{
	return creditRepo->addCreditEntry(
		entry::Credit{ 
			.creditEntryID = 0, 
			.personID = personID, 
			.date = date, 
			.amount = amount, 
			.description = description 
		});
}

std::vector<entry::Balance> BalanceService::getEntries(BalanceType type) const
{
	auto entries = balanceRepo->getBalanceEntries(type);

	if (type == BalanceType::EarningAndSupplement)
	{
		entries.push_back(
			entry::Balance{
				.type = BalanceType::Earning,
				.description = "Einnahmen durch Getränkeverkäufe",
				.amount = debtRepo->getTotalShare(FinancialShare::Own),
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
	
	for (const auto& entry : getEntries(BalanceType::Earning)) departmentEarnings += entry.amount;
	for (const auto& entry : getEntries(BalanceType::Spending)) departmentSpendings += entry.amount;
	consumptionOwnShare = debtRepo->getTotalShare(FinancialShare::Own);


	double savingsDiff = departmentEarnings - departmentSpendings + consumptionOwnShare;


	// cashDiff = (departmentEarnings - departmentSpendings) + (paidDebt (which is totalShare) - settledValue (which is foreignShare)) + depositedCredit
	double paidDebt{};
	double settledValue{};
	double depositedCredit{};

	paidDebt = paymentRepo->getTotalAllocatedPayments();
	settledValue = shareSettlementRepo->getTotalAllocatedShareSettlements();
	depositedCredit = creditRepo->getTotalDepositedCredit();


	double cashDiff = departmentEarnings - departmentSpendings + paidDebt - settledValue + depositedCredit;

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
		.shareSettlementID = 0,
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
			.shareSettlementAllocationID = 0,
			.debtEntryID = entryRem.debtEntryID,
			.shareSettlementID = settlementEntryID,
			.amount = appliedToCurrentEntry 
		};

		shareSettlementRepo->addShareSettlementAllocationEntry(aEntry);
	}

	return amountLeft;
}