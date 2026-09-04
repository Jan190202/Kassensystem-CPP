#include "PaymentService.h"

PaymentService::PaymentService(PaymentRepository* paymentRepo, CreditRepository* creditRepo, DebtRepository* debtRepo, ConsumptionRepository* consumptionRepo, BalanceRepository* balanceRepo, PersonRepository* personRepo)
	: paymentRepo(paymentRepo), creditRepo(creditRepo), debtRepo(debtRepo), consumptionRepo(consumptionRepo), balanceRepo(balanceRepo), personRepo(personRepo) {}

void PaymentService::addPayment(const PaymentRequest& request)
{
	if (request.amount < 1e-9) return;

	entry::Payment entry{
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

double PaymentService::addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount, QDate date)
{
	std::vector<entry::DebtRemaining> remainingDebtEntries = debtRepo->getOutstandingEntries(personID, FilterType::OmitFullyPaid);
	
	double amountLeft = amount;
	for (auto& entryRem : remainingDebtEntries)
	{
		if (amountLeft < 1e-9) break;

		double appliedToCurrentEntry = std::min(amountLeft, entryRem.remaining);

		amountLeft -= appliedToCurrentEntry;

		entry::PaymentAllocation aEntry{ 
			.paymentAllocationEntryID = 0, 
			.debtEntryID = entryRem.debtEntryID, 
			.paymentEntryID = paymentEntryID, 
			.date = date, 
			.amount = appliedToCurrentEntry };

		paymentRepo->addAllocationEntry(aEntry);
	}

	return amountLeft;
}

int64_t PaymentService::addCredit(int64_t personID, double amount, QDate date, std::string description)
{
	return creditRepo->addEntry(
		entry::Credit{ 
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
		entry::Balance{ 
		.balanceEntryID = 0, 
		.type = BalanceType::Earning, 
		.description = "Trinkgeld", 
		.amount = amount, 
		.date = date, 
		.comment = "", 
		.personID = personID }
		);
}


// TBD: get rid of passthrough functions
double PaymentService::getSettledAmount(int64_t personID) const
{
	return debtRepo->getSettled(personID);
}

double PaymentService::getTotalAmount(int64_t personID) const
{
	return debtRepo->getTotal(personID);
}

double PaymentService::getCreditAmount(int64_t personID) const
{
	return creditRepo->getCredit(personID);
}

double PaymentService::getDueAmount(int64_t personID) const
{
	return debtRepo->getDue(personID);
}

std::vector<entry::Consumption> PaymentService::getConsumptionEntries(int64_t personID)
{
	return consumptionRepo->getEntries(personID);
}

std::vector<entry::DebtRemaining> PaymentService::getOutstandingEntries(int64_t personID, FilterType filter)
{
	return debtRepo->getOutstandingEntries(personID, filter);
}

void PaymentService::resetCredit(int64_t personID)
{
	creditRepo->resetCredit(personID);
}