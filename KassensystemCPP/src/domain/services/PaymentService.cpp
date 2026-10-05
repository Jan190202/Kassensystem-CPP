#include "PaymentService.h"
#include <expected>
#include <algorithm>

PaymentService::PaymentService(const RepositoryBundle& repoBundle, PendingChangeLog& log)
	: paymentRepo(repoBundle.paymentRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo), consumptionRepo(repoBundle.consumptionRepo), balanceRepo(repoBundle.balanceRepo), personRepo(repoBundle.personRepo), log(log) {}

void PaymentService::addPayment(const request::Payment& request)
{
	if (request.amount < 1e-9) return;

	entry::Payment entry{
		.paymentEntryID = 0,
		.person = request.person,
		.dateAdded= QDate::currentDate(),
		.amount = request.amount,
		.comment = request.comment,
		.overpaymentType = request.overpaymentType
	};

	int64_t paymentEntryID = paymentRepo->addPaymentEntry(entry);
	double overpaymentAmount = addPaymentAllocation(paymentEntryID, entry.person.personEntryID, entry.amount);

	log.record(PendingChangeLog::ChangeType::add, entry);

	if (overpaymentAmount > 1e-9)
	{
		switch (entry.overpaymentType)
		{
		case OverpaymentDisposition::credit:
			addCredit(entry.person, overpaymentAmount, entry.dateAdded, "Guthaben durch Einzahlung/Überbezahlung");
			break;
		case OverpaymentDisposition::tip:
			addTip(entry.person, overpaymentAmount, entry.dateAdded);
			break;
		}
	}
}

double PaymentService::addPaymentAllocation(int64_t paymentEntryID, int64_t personEntryID, double amount)
{
	std::vector<entry::Outstanding> remainingDebtEntries = debtRepo->getPersonsOutstandingEntries(personEntryID, FilterType::omitFullyPaid);
	
	// sort be reverse date so older outstanding debts are paid first
	std::sort(remainingDebtEntries.begin(), remainingDebtEntries.end(), [](const entry::Outstanding& a, const entry::Outstanding& b)
		{
			return a.dateBooked < b.dateBooked;
		});

	double amountLeft = amount;
	for (const auto& entryRem : remainingDebtEntries)
	{
		if (amountLeft < 1e-9) break;

		double appliedToCurrentEntry = std::min(amountLeft, entryRem.remaining);

		amountLeft -= appliedToCurrentEntry;

		entry::PaymentAllocation aEntry{ 
			.paymentAllocationEntryID = 0, 
			.debtEntryID = entryRem.debtEntryID, 
			.paymentEntryID = paymentEntryID, 
			.amount = appliedToCurrentEntry };

		paymentRepo->addPaymentAllocationEntry(aEntry);
	}

	return amountLeft;
}

int64_t PaymentService::addCredit(const request::Credit& request)
{
	return addCredit(request.person, request.amount, request.dateBooked, request.description);
}

int64_t PaymentService::addCredit(const entry::Person& person, double amount, const RegisterDate& date, const std::string& description)
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

int64_t PaymentService::addTip(const entry::Person& person, double amount, const RegisterDate& date)
{
	// potential validity check here
	
	auto entry = entry::Balance{
		.balanceEntryID = 0,
		.type = BalanceType::earning,
		.description = "Trinkgeld bei Schuldenbegleichung",
		.amount = amount,
		.dateBooked = date,
		.dateAdded = QDate::currentDate(),
		.comment = "",
		.person = person
	};

	log.record(PendingChangeLog::ChangeType::add, entry);
	return balanceRepo->addBalanceEntry(entry);
}

std::vector<exportType::PersonDebt> PaymentService::getDebtsAll() const
{
	auto personVec = personRepo->getAllPersonEntries();
	
	std::vector<exportType::PersonDebt> debtEntries;
	debtEntries.reserve(personVec.size());

	for (const auto& entry : personVec)
	{
		debtEntries.emplace_back(exportType::PersonDebt{
			.name = entry.getFullName(),
			.debt = debtRepo->getPersonsDue(entry.personEntryID) - creditRepo->getPersonsCredit(entry.personEntryID)
			});
	}

	return debtEntries;
}