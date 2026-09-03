#include "gui/CashRegisterSystemUI.h"
#include "gui/GuiTypes.h"
#include <QApplication>

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/model/DomainTypes.h"

#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include "domain/services/BalanceService.h"

#include "test/BalanceRepoInMem.h"
#include "test/ConsumptionRepoInMem.h"
#include "test/CreditRepoInMem.h"
#include "test/DebtRepoInMem.h"
#include "test/PaymentRepoInMem.h"
#include "test/PersonRepoInMem.h"

#include <string>
#include <QDebug>
#include <iostream>
#include <optional>

int main(int argc, char* argv[])
{
	BalanceRepository* baRep		= new BalanceRepoInMem();
	ConsumptionRepository* coRep	= new ConsumptionRepoInMem();
	CreditRepository* crRep			= new CreditRepoInMem();
	DebtRepository* deRep			= new DebtRepoInMem();
	PaymentRepository* paRep		= new PaymentRepoInMem();
	PersonRepository* peRep			= new PersonRepoInMem();

	BalanceService			baSer(baRep, crRep, peRep);
	ConsumptionService		coSer(coRep, deRep, peRep);
	PaymentService			paSer(paRep, crRep, deRep, coRep, baRep, peRep);

	// domain testing
	int64_t p1ID = peRep->addEntry("Tim", "Ebert").getID();
	int64_t p2ID = peRep->addEntry("Alfons", "Strauss").getID();

	ConsumptionRequest cReq4{ .personInput = p1ID, .date = QDate::currentDate(), .nBeer05 = 6, .nBeer04 = 9, .nSoftdrinks = 3, .nWater = 1, .otherExpense = 1.4 };
	ConsumptionRequest cReq1{ .personInput = "Dieter Armen", .date = QDate::currentDate(), .nBeer05 = 1, .nBeer04 = 2, .nSoftdrinks = 3, .nWater = 1, .otherExpense = 1.4 };
	ConsumptionRequest cReq2{ .personInput = "Maja Apfel",	 .date = QDate::currentDate(), .nBeer05 = 2, .nBeer04 = 2, .nSoftdrinks = 5, .nWater = 5, .otherExpense = 3.1 };
	ConsumptionRequest cReq3{ .personInput = "Max Birne",    .date = QDate::currentDate(), .nBeer05 = 0, .nBeer04 = 1, .nSoftdrinks = 0, .nWater = 0, .otherExpense = 1 };

	BalanceRequest bReq1{ .type = BalanceType::Spending, .description = "", .amount = 11, .date = QDate::currentDate(), .comment = "", .coveringPersonID = std::nullopt };
	BalanceRequest bReq2{ .type = BalanceType::Earning,  .description = "", .amount = 200, .date = QDate::currentDate(), .comment = "", .coveringPersonID = std::nullopt };
	BalanceRequest bReq3{ .type = BalanceType::Spending, .description = "", .amount = 13, .date = QDate::currentDate(), .comment = "", .coveringPersonID = p2ID };

	PaymentRequest pEntry1{ .personID = p1ID, .date = QDate::currentDate(), .amount = 20, .overpaymentType = OverpaymentDisposition::Credit };
	PaymentRequest pEntry2{ .personID = p1ID, .date = QDate::currentDate(), .amount = 800, .overpaymentType = OverpaymentDisposition::Tip };


	coSer.addConsumption(cReq1);
	coSer.addConsumption(cReq2);
	coSer.addConsumption(cReq3);
	coSer.addConsumption(cReq4);

	baSer.addEntry(bReq1);
	baSer.addEntry(bReq2);
	baSer.addEntry(bReq3);

	paSer.addPayment(pEntry1);
	paSer.addPayment(pEntry2);



	// GUI Testing
	QApplication app(argc, argv);
	auto services = ServiceBundle{ .paymentService = paSer, .consumptionService = coSer, .balanceService = baSer, .personRepo = peRep};
	CashRegisterSystemUI sysUI(services);
	sysUI.show();



	return app.exec();
}