#include "BalanceService.h"
#include "domain/model/FinancialStateBefore.h"

#include <optional>

BalanceService::BalanceService(BalanceRepository* balanceRepo, CreditRepository* creditRepo, DebtRepository* debtRepo, PersonRepository* personRepo) 
	: balanceRepo(balanceRepo), creditRepo(creditRepo), debtRepo(debtRepo), personRepo(personRepo) {}

int64_t BalanceService::addEntry(const BalanceRequest& request)
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
	return balanceRepo->getEntries(type);
}

registerFinancials::Report BalanceService::getReport() const
{
	// calculations
	double totalEarnings = balanceRepo->getTotalEarnings();
	double totalSpendings = balanceRepo->getTotalSpendings();

	double savingsDiff = totalEarnings - totalSpendings;
	double totalDebt = debtRepo->getTotal(FinancialShare::All);
	double totalCredit = creditRepo->getTotal();
	double cashDiff = savingsDiff - totalDebt + totalCredit;

	double currentForeignCash = debtRepo->getTotal(FinancialShare::Foreign);

	// struct construction
	registerFinancials::State stateBefore = financialStateBefore::read();

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
