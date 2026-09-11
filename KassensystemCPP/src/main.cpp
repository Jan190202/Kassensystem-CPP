#include "gui/CashRegisterSystemUI.h"
#include "gui/GuiTypes.h"

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
#include "test/ShareSettlementRepoInMem.h"
#include "test/PaymentRepoInMem.h"
#include "test/PersonRepoInMem.h"

#include "data/config/PriceListLoader.h"
#include "data/config/FinancialStateLoader.h"

#include "app/RepositoryBundle.h"
#include "app/ServiceBundle.h"

#include <string>
#include <iostream>
#include <optional>

#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
	// initialize QApp
	QApplication app(argc, argv);

	// load read-only data
	const PriceList priceList = priceListLoader::read();
	const registerFinancials::State financialStateBefore = financialStateLoader::read();

	// initialize repositories and services
	PersonRepository* peRep				= new PersonRepoInMem();
	ConsumptionRepository* coRep		= new ConsumptionRepoInMem();
	PaymentRepository* paRep			= new PaymentRepoInMem();
	CreditRepository* crRep				= new CreditRepoInMem();
	BalanceRepository* baRep			= new BalanceRepoInMem();
	ShareSettlementRepository* seRep	= new ShareSettlementRepoInMem();
	DebtRepository* deRep				= new DebtRepoInMem(paRep, seRep);
	RepositoryBundle repoBundle{ .personRepo = peRep, .consumptionRepo = coRep, .debtRepo = deRep, .paymentRepo = paRep, .creditRepo = crRep, .balanceRepo = baRep, .shareSettlementRepo = seRep };

	ConsumptionService		coSer(repoBundle, priceList);
	BalanceService			baSer(repoBundle, financialStateBefore);
	PaymentService			paSer(repoBundle);
	ServiceBundle serviceBundle{ .consumptionService = coSer, .paymentService = paSer, .balanceService = baSer };

	// domain testing
	int64_t p1ID = peRep->addPersonEntry(entry::Person{ .firstName = "Tim", .lastName = "Ebert" });
	int64_t p2ID = peRep->addPersonEntry(entry::Person{ .firstName = "Alfons", .lastName = "Strauss" });

	request::Consumption cReq4{ .personInput = p1ID, .date = QDate::currentDate(), .nBeer05 = 6, .nBeer04 = 9, .nSoftdrinks = 3, .nWater = 1, .otherExpense = 1.4 };
	request::Consumption cReq5{ .personInput = p1ID, .date = QDate::currentDate(), .nBeer05 = 60, .nBeer04 = 9, .nSoftdrinks = 3, .nWater = 1, .otherExpense = 1.4 };
	//request::Consumption cReq1{ .personInput = "Dieter Armen", .date = QDate::currentDate(), .nBeer05 = 1, .nBeer04 = 2, .nSoftdrinks = 3, .nWater = 1, .otherExpense = 1.4 };
	//request::Consumption cReq2{ .personInput = "Maja Apfel",	 .date = QDate::currentDate(), .nBeer05 = 2, .nBeer04 = 2, .nSoftdrinks = 5, .nWater = 5, .otherExpense = 3.1 };
	//request::Consumption cReq3{ .personInput = "Max Birne",    .date = QDate::currentDate(), .nBeer05 = 0, .nBeer04 = 1, .nSoftdrinks = 0, .nWater = 0, .otherExpense = 1 };

	request::Balance bReq1{ .type = BalanceType::Spending, .description = "Eintrag 1", .amount = 11, .date = QDate::currentDate(), .comment = "", .coveringPersonID = std::nullopt };
	request::Balance bReq2{ .type = BalanceType::Earning,  .description = "Eintrag 2", .amount = 200, .date = QDate::currentDate(), .comment = "", .coveringPersonID = std::nullopt };
	request::Balance bReq3{ .type = BalanceType::Spending, .description = "Eintrag 3", .amount = 13, .date = QDate::currentDate(), .comment = "", .coveringPersonID = p2ID };

	request::Payment pEntry1{ .personID = p1ID, .date = QDate::currentDate(), .amount = 20, .overpaymentType = OverpaymentDisposition::Credit };
	request::Payment pEntry2{ .personID = p1ID, .date = QDate::currentDate(), .amount = 80, .overpaymentType = OverpaymentDisposition::Tip };


	//coSer.addConsumption(cReq1);
	//coSer.addConsumption(cReq2);
	//coSer.addConsumption(cReq3);
	coSer.addConsumption(cReq4);

	baSer.addBalanceItem(bReq1);
	baSer.addBalanceItem(bReq2);
	baSer.addBalanceItem(bReq3);

	paSer.addPayment(pEntry1);
	//paSer.addPayment(pEntry2);

	coSer.addConsumption(cReq5);


	// start UI
	CashRegisterSystemUI sysUI(serviceBundle, repoBundle);
	sysUI.show();

	return app.exec();
}

/*
* TBD:
* - repo interactions (getter, calculation, ...) always restricted to date >= dateBefore (from FinancialStateBefore)
* - repo controller: apply (temporary save), save (full save and sync)
*/

/*
* Ideas:
* - apply (save) button only active when changes were made
* - new button: sync -> save saves to local copy of database, sync pushes it to remote
* - add little calculator in balanceTab for cash counting
* - add buttons in PayTab: "Redeem All Credits", "Export Debts (CSV/CopyToClipboard/URL)"
* - add info-icons in BalanceTab (clickable/tooltip) -> get details on report calculation
* - add new tab for manual database changes (dropDown for which repository + QTableView)
*/ 