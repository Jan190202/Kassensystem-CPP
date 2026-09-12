#include "PaymentService.h"
#include <expected>
#include <algorithm>

PaymentService::PaymentService(const RepositoryBundle& repoBundle)
	: paymentRepo(repoBundle.paymentRepo), creditRepo(repoBundle.creditRepo), debtRepo(repoBundle.debtRepo), consumptionRepo(repoBundle.consumptionRepo), balanceRepo(repoBundle.balanceRepo), personRepo(repoBundle.personRepo) {}

void PaymentService::addPayment(const request::Payment& request)
{
	if (request.amount < 1e-9) return;

	entry::Payment entry{
		.paymentEntryID = 0,
		.personEntryID = request.personEntryID,
		.date = request.date,
		.amount = request.amount,
		.overpaymentType = request.overpaymentType
	};

	int64_t paymentEntryID = paymentRepo->addPaymentEntry(entry);
	double overpaymentAmount = addPaymentAllocation(paymentEntryID, entry.personEntryID, entry.amount);

	if (overpaymentAmount > 1e-9)
	{
		switch (entry.overpaymentType)
		{
		case OverpaymentDisposition::Credit:
			addCredit(entry.personEntryID, overpaymentAmount, entry.date, "Guthaben durch Einzahlung/Überbezahlung");
			break;
		case OverpaymentDisposition::Tip:
			addTip(entry.personEntryID, overpaymentAmount, entry.date);
			break;
		}
	}
}

double PaymentService::addPaymentAllocation(int64_t paymentEntryID, int64_t personEntryID, double amount)
{
	std::vector<entry::Outstanding> remainingDebtEntries = debtRepo->getPersonsOutstandingEntries(personEntryID, FilterType::OmitFullyPaid);
	
	// sort be reverse date so older outstanding debts are paid first
	std::sort(remainingDebtEntries.begin(), remainingDebtEntries.end(), [](const entry::Outstanding& a, const entry::Outstanding& b)
		{
			return a.date < b.date;
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

int64_t PaymentService::addCredit(int64_t personEntryID, double amount, const QDate& date, const std::string& description)
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

int64_t PaymentService::addTip(int64_t personEntryID, double amount, const QDate& date)
{
	// potential validity check here
	
	return balanceRepo->addBalanceEntry(
		entry::Balance{ 
		.balanceEntryID = 0, 
		.type = BalanceType::Earning, 
		.description = "Trinkgeld bei Schuldenbegleichung",
		.amount = amount, 
		.dateBooked = date, 
		.dateAdded = QDate::currentDate(),
		.comment = "", 
		.personEntryID = personEntryID 
		});
}