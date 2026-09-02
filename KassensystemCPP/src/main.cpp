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
	PaymentService			paSer(paRep, crRep, deRep, baRep, peRep);

	// domain testing
	peRep->addEntry("Tim", "Ebert");
	peRep->addEntry("Alfons", "Strauß");
	//Person p1("Dieter", 1);
	//Person p2("Gerhardt", 2);
	//Person p3("Udo", 3);
	//Person p4("Sabine", 4);
	//Person p5("Laura", 5);

	//ConsumptionRequest cReq1{ .personName = "Dieter", .date = QDate::currentDate(), .nBeer05 = 1, .nBeer04 = 2, .nSoftdrinks = 3, .nWater = 1, .otherExpense = 1.4 };
	//ConsumptionRequest cReq2{ .personName = "Maja",	 .date = QDate::currentDate(), .nBeer05 = 2, .nBeer04 = 2, .nSoftdrinks = 5, .nWater = 5, .otherExpense = 3.1 };
	//ConsumptionRequest cReq3{ .personName = "Max",    .date = QDate::currentDate(), .nBeer05 = 0, .nBeer04 = 1, .nSoftdrinks = 0, .nWater = 0, .otherExpense = 1 };

	//BalanceRequest bReq1{ .type = BalanceType::spending, .description = "", .amount = 11, .date = QDate::currentDate(), .comment = "", .isCovered = false, .personName = "" };
	//BalanceRequest bReq2{ .type = BalanceType::earning,  .description = "", .amount = 200, .date = QDate::currentDate(), .comment = "", .isCovered = false, .personName = "" };
	//BalanceRequest bReq3{ .type = BalanceType::spending, .description = "", .amount = 1, .date = QDate::currentDate(), .comment = "", .isCovered = true, .personName = "Tim" };

	//PaymentEntry pEntry1{ .paymentEntryID = 0, .personID = 1, .date = QDate::currentDate(), .amount = 100, .overpaymentType = OverpaymentDisposition::credit };
	//PaymentEntry pEntry2{ .paymentEntryID = 0, .personID = 2, .date = QDate::currentDate(), .amount = 200, .overpaymentType = OverpaymentDisposition::tip };

	//baSer.addEntry(bReq1);
	//baSer.addEntry(bReq2);
	//baSer.addEntry(bReq3);
	//qInfo() << baSer.getTotalSpendings();
	//qInfo() << baSer.getTotalEarnings();
	//coSer.addConsumption(cReq1);
	//coSer.addConsumption(cReq2);
	//coSer.addConsumption(cReq3);
	//coSer.getEntries(peRep->findOrCreateEntry("Dieter").getID());
	//paSer.addPayment(pEntry1);
	//paSer.addPayment(pEntry2);
	//qInfo() << baSer.getTotalEarnings();



	// GUI Testing
	QApplication app(argc, argv);
	auto services = ServiceBundle{ .paymentService = paSer, .consumptionService = coSer, .balanceService = baSer, .personRepo = peRep};
	CashRegisterSystemUI sysUI(services);
	sysUI.show();



	return app.exec();
}