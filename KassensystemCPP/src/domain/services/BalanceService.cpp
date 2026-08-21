#include "BalanceService.h"

BalanceService::BalanceService(BalanceRepository* balanceRepo, CreditRepository* creditRepo, PersonRepository* personRepo)
{
	this->balanceRepo = balanceRepo;
	this->creditRepo = creditRepo;
	this->personRepo = personRepo;
}

int64_t BalanceService::addEntry(const BalanceRequest& request)
{
	BalanceEntry entry{
		.BalanceEntryID = 0,
		.type = request.type,
		.description = request.description,
		.amount = request.amount,
		.date = request.date,
		.comment = request.comment,
		.personID = personRepo->findOrCreateEntry(request.personName).getID()
	};

	if (entry.personID >= 0 && entry.type == BalanceType::spending)
	{
		addCredit(entry.personID, entry.amount, entry.date, "Abteilungsausgabe übernommen");
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
		CreditEntry{ 
			.creditEntryID = 0, 
			.personID = personID, 
			.date = date, 
			.amount = amount, 
			.description = description }
			);
}