#include "PaymentService.h"

PaymentService::PaymentService(PaymentRepository* paymentRepo, CreditRepository* creditRepo, DebtRepository* debtRepo, BalanceRepository* balanceRepo, PersonRepository* personRepo)
	: paymentRepo(paymentRepo), creditRepo(creditRepo), debtRepo(debtRepo), balanceRepo(balanceRepo), personRepo(personRepo) {}

void PaymentService::addPayment(const PaymentRequest& request)
{
	if (request.amount < 1e-9) return;

	PaymentEntry entry{
		.paymentEntryID = 0,
		.personID = request.personID,
		.date = request.date,
		.amount = request.amount,
		.overpaymentType = request.overpaymentType
	};

	int64_t paymentEntryID = paymentRepo->addPaymentEntry(entry);
	double overpaymentAmount = addPaymentAllocation(paymentEntryID, entry.personID, entry.amount, entry.date);

	if (overpaymentAmount > 1e-9)
	{
		switch (entry.overpaymentType)
		{
		case OverpaymentDisposition::Credit:
			addCredit(entry.personID, entry.amount, entry.date, "Guthaben durch Einzahlung/Überbezahlung");
			break;
		case OverpaymentDisposition::Tip:
			addTip(entry.personID, entry.amount, entry.date);
			break;
		}
	}
}

double PaymentService::getPaidAmount(int64_t personID) const
{
	return paymentRepo->getPaidAmount(personID);
}

double PaymentService::getTotalAmount(int64_t personID) const
{
	return debtRepo->getTotal(personID);
}

double PaymentService::getCreditAmount(int64_t personID) const
{
	return creditRepo->getCredit(personID);
}

double PaymentService::addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount, QDate date)
{
	std::vector<DebtEntryRemaining> remainingDebtEntries = debtRepo->getOutstandingEntries(personID);
	
	double amountLeft = amount;
	for (auto& entryRem : remainingDebtEntries)
	{
		if (amountLeft < 1e-9) break;

		double appliedToCurrentEntry = std::min(amountLeft, entryRem.remaining);

		amountLeft -= appliedToCurrentEntry;

		PaymentAllocationEntry aEntry{ 
			.paymentAllocationEntryID = 0, 
			.entryID = entryRem.entryID, 
			.paymentID = paymentEntryID, 
			.date = date, 
			.amount = appliedToCurrentEntry };

		paymentRepo->addAllocationEntry(aEntry);
	}

	return amountLeft;
}

int64_t PaymentService::addCredit(int64_t personID, double amount, QDate date, std::string description)
{
	return creditRepo->addEntry(
		CreditEntry{ 
			.creditEntryID = 0, 
			.personID = personID, 
			.date = date, 
			.amount = amount, 
			.description = description}
			);
}

int64_t PaymentService::addTip(int64_t personID, double amount, QDate date)
{
	return balanceRepo->addEntry(
		BalanceEntry{ 
		.BalanceEntryID = 0, 
		.type = BalanceType::Earning, 
		.description = "Trinkgeld", 
		.amount = amount, 
		.date = date, 
		.comment = "", 
		.personID = personID }
		);
}