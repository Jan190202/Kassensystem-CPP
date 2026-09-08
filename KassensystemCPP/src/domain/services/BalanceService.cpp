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

		entries.push_back(
			entry::Balance{
				.type = BalanceType::Earning,
				.description = "Rundungsfehler bei Abrechnung (kum.)",
				.amount = 0, //settlementRepo->getTotalRounding(),
				.dateBooked = QDate::currentDate()
			});
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

void BalanceService::settleForeignShare()
{
	// settlementRepo->settleForeignShare();
}