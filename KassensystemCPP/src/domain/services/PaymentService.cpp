#include "PaymentService.h"

PaymentService::PaymentService(PaymentRepository* paymentRepo, CreditRepository* creditRepo, DebtRepository* debtRepo, BalanceRepository* balanceRepo, PersonRepository* personRepo)
{
	this->paymentRepo = paymentRepo;
	this->creditRepo = creditRepo;
	this->debtRepo = debtRepo;
	this->balanceRepo = balanceRepo;
	this->personRepo = personRepo;
}

void PaymentService::addPayment(const PaymentRequest& request)
{
	if (request.amount < 1e-9) return;

	PaymentEntry entry{
		.paymentEntryID = 0,
		.personID = personRepo->findOrCreateEntry(request.personName).getID(),
		.date = request.date,
		.amount = request.amount,
		.overpaymentType = request.overpaymentType
	};

	int64_t paymentEntryID = addPaymentEntry(entry);
	double overpaymentAmount = addPaymentAllocation(paymentEntryID, entry.personID, entry.amount, entry.date);

	if (overpaymentAmount > 1e-9)
	{
		switch (entry.overpaymentType)
		{
		case OverpaymentDisposition::credit:
			addCredit(entry.personID, entry.amount, entry.date, "Guthaben durch Einzahlung/Überbezahlung");
			break;
		case OverpaymentDisposition::tip:
			addTip(entry.personID, entry.amount, entry.date);
			break;
		}
	}
}

double PaymentService::getPaidAmount(int64_t personID) const
{
	return paymentRepo->getPaidAmount(personID);
}

int64_t PaymentService::addPaymentEntry(const PaymentEntry& entry)
{
	return paymentRepo->addPaymentEntry(entry);
}

double PaymentService::addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount, QDate date)
{
	std::vector<DebtEntryRemaining> remainingDebtEntries = debtRepo->getOutstanding(personID);
	
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
		.type = BalanceType::earning, 
		.description = "Trinkgeld", 
		.amount = amount, 
		.date = date, 
		.comment = "", 
		.personID = personID }
		);
}