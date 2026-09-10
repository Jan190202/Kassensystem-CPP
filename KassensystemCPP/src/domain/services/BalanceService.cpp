#include "BalanceService.h"

#include <optional>

BalanceService::BalanceService(BalanceRepository* balanceRepo, CreditRepository* creditRepo, DebtRepository* debtRepo, PersonRepository* personRepo, SettlementRepository* settlementRepo, const registerFinancials::State& stateBefore)
	: balanceRepo(balanceRepo), creditRepo(creditRepo), debtRepo(debtRepo), personRepo(personRepo), settlementRepo(settlementRepo), stateBefore(stateBefore) {}

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
	// calculations
	double totalEarnings{};
	auto entries = getEntries(BalanceType::EarningAndSupplement);
	for (const auto& entry : entries) totalEarnings += entry.amount;
		
	double totalSpendings{};
	entries = getEntries(BalanceType::Spending);
	for (const auto& entry : entries) totalSpendings += entry.amount;

	double savingsDiff = totalEarnings - totalSpendings;
	double totalDebt = debtRepo->getTotal(FinancialShare::All);
	double totalCredit = creditRepo->getTotal();
	double cashDiff = savingsDiff - totalDebt + totalCredit; // TBD: settlements also decrease cash

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
		.totalEarnings = totalEarnings,
		.totalSpendings = totalSpendings
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