#include "BalanceService.h"

#include <optional>

BalanceService::BalanceService(BalanceRepository* balanceRepo, CreditRepository* creditRepo, PersonRepository* personRepo)
{
	this->balanceRepo = balanceRepo;
	this->creditRepo = creditRepo;
	this->personRepo = personRepo;
}

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

	if (entry.personID >= 0 && entry.type == BalanceType::Spending)
	{
		addCredit(entry.personID, entry.amount, entry.dateBooked, "Abteilungsausgabe übernommen");
	}
	
	return balanceRepo->addEntry(entry);
}

double BalanceService::getTotalEarnings() const
{
	return balanceRepo->getTotalEarnings();
}

double BalanceService::getTotalSpendings() const
{
	return balanceRepo->getTotalSpendings();
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