#include "BalanceService.h"
#include <optional>

BalanceService::BalanceService(const RepositoryBundle& repoBundle, const registerFinancials::State& stateBefore)
	: balanceRepo(repoBundle.balanceRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo), personRepo(repoBundle.personRepo), settlementRepo(repoBundle.settlementRepo), paymentRepo(repoBundle.paymentRepo), stateBefore(stateBefore) {}

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
	
	return balanceRepo->addEntry(entry);
}

int64_t BalanceService::addCredit(int64_t personID, double amount, QDate date, std::string description)
{
	return creditRepo->addEntry(
		entry::Credit{ 
			.creditEntryID = 0, 
			.personID = personID, 
			.date = date, 
			.amount = amount, 
			.description = description }
			);
}

std::vector<entry::Balance> BalanceService::getEntries(BalanceType type) const
{
	auto entries = balanceRepo->getEntries(type);

	if (type == BalanceType::EarningAndSupplement)
	{
		entries.push_back(
			entry::Balance{
				.type = BalanceType::Earning,
				.description = "Einnahmen durch Getränkeverkäufe",
				.amount = debtRepo->getTotal(FinancialShare::Own),
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
	consumptionOwnShare = debtRepo->getTotal(FinancialShare::Own);


	double savingsDiff = departmentEarnings - departmentSpendings + consumptionOwnShare;


	// cashDiff = (departmentEarnings - departmentSpendings) + (paidDebt (which is totalShare) - settledValue (which is foreignShare)) + accumulatedCredit
	double paidDebt{};
	double settledValue{};
	double accumulatedCredit{};

	paidDebt = paymentRepo->getPaidAllocTotal();
	settledValue = settlementRepo->getTotal();
	accumulatedCredit = creditRepo->getTotal();


	double cashDiff = departmentEarnings - departmentSpendings + paidDebt - settledValue + accumulatedCredit;

	double currentForeignCash = debtRepo->getDue();





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

AddSettlementException BalanceService::addSettlement(request::Settlement request)
{
	if (request.amount < 0) 
		return AddSettlementException::AmountNegative;
	if (request.amount < 1e-9) 
		return AddSettlementException::AmountZero;
	if (request.amount - debtRepo->getDue() > 1e-9)
		return AddSettlementException::AmountGreaterThanTotalForeignShare;

	entry::Settlement entry{
		.settlementID = 0,
		.date = QDate::currentDate(),
		.amount = request.amount,
	};

	int64_t settlementEntryID = settlementRepo->addSettlementEntry(entry);

	double overpaymentAmount = addSettlementAllocation(settlementEntryID, entry.amount);

	if (overpaymentAmount > 1e-9)
	{
		// case currently caught with AddSettlementException::AmountGreaterThantTotalForeignShare
		// if settlement in advance (or up-rounding) intended later, implement here
	}

	debtRepo->getSettlementOutstandingEntries(FilterType::OmitFullyPaid);

	return AddSettlementException::None;
}

double BalanceService::addSettlementAllocation(int64_t settlementEntryID, double amount)
{
	std::vector<entry::Outstanding> remainingDebtEntries = debtRepo->getSettlementOutstandingEntries(FilterType::OmitFullyPaid);

	double amountLeft = amount;
	for (const auto& entryRem : remainingDebtEntries)
	{
		if (amountLeft < 1e-9) break;

		double appliedToCurrentEntry = std::min(amountLeft, entryRem.remaining);

		amountLeft -= appliedToCurrentEntry;

		entry::SettlementAllocation aEntry{
			.settlementAllocationID = 0,
			.debtEntryID = entryRem.debtEntryID,
			.settlementID = settlementEntryID,
			.amount = appliedToCurrentEntry 
		};

		settlementRepo->addSettlementAllocationEntry(aEntry);
	}

	return amountLeft;
}