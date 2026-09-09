#include "PaymentService.h"

#include <expected>

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
			addCredit(entry.personID, overpaymentAmount, entry.date, "Guthaben durch Einzahlung/Überbezahlung");
			break;
		case OverpaymentDisposition::Tip:
			addTip(entry.personID, overpaymentAmount, entry.date);
			break;
		}
	}
}

double PaymentService::addPaymentAllocation(int64_t paymentEntryID, int64_t personID, double amount, QDate date)
{
	std::vector<entry::Outstanding> remainingDebtEntries = debtRepo->getPaymentOutstandingEntries(personID, FilterType::OmitFullyPaid);
	
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
	//std::string tipDescription = "Trinkgeld bei Schuldenbegleichung";
	//double existingTips{};

	//auto tipEntry = balanceRepo->getEntry(tipDescription);
	//
	//if (tipEntry.has_value())
	//{
	//	existingTips += tipEntry.value().amount;
	//	balanceRepo->removeEntry(tipEntry.value().balanceEntryID);
	//}

	return balanceRepo->addEntry(
		entry::Balance{ 
		.balanceEntryID = 0, 
		.type = BalanceType::Earning, 
		.description = "Trinkgeld bei Schuldenbegleichung",
		.amount = amount, 
		.dateBooked = date, 
		.dateAdded = QDate::currentDate(),
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

std::vector<entry::Outstanding> PaymentService::getPaymentOutstandingEntries(int64_t personID, FilterType filter)
{
	return debtRepo->getPaymentOutstandingEntries(personID, filter);
}

void PaymentService::resetCredit(int64_t personID)
{
	creditRepo->resetCredit(personID);
}