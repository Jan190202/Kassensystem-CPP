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

#include "system/SystemConfig.h"

#include <string>
#include <iostream>
#include <optional>
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
	systemConfig::setUTF8Encoding();

	// initialize QApp
	QApplication app(argc, argv);

	// load read-only data
	qDebug() << "Reading config:";
	const PriceList priceList = priceListLoader::read();
	const registerFinancials::State financialStateBefore = financialStateLoader::read();
	qDebug() << "\n";

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
	qDebug() << "Adding person entries";
	int64_t p1ID = peRep->addPersonEntry(entry::Person{ .firstName = "Tim", .lastName = "Ebert" });
	int64_t p2ID = peRep->addPersonEntry(entry::Person{ .firstName = "Alfons", .lastName = "Strauß", .info = "Gast"});
	int64_t p3ID = peRep->addPersonEntry(entry::Person{ .firstName = "Eberhadt", .lastName = "Nöbel", .nickName = "Eber"});
	int64_t p4ID = peRep->addPersonEntry(entry::Person{ .firstName = "Dieter", .lastName = "Armen"});
	qDebug() << "\n"; 

	// total,allDates: 80 (12+68), totalShare,>=2026: 40 (6+34)
	request::Consumption cReq1{ .personInput = p4ID, .date = QDate::currentDate(), .otherExpense = 10};
	request::Consumption cReq2{ .personInput = p4ID, .date = QDate(2025,1,1), .otherExpense = 10};
	request::Consumption cReq3{ .personInput = p4ID, .date = QDate(2025,7,7), .otherExpense = 10};
	request::Consumption cReq4{ .personInput = p4ID, .date = QDate(2025,12,31), .otherExpense = 10};
	request::Consumption cReq5{ .personInput = "Maja Apfel", .date = QDate(2026,1,1), .otherExpense = 10};
	request::Consumption cReq6{ .personInput = "Max Birne", .date = QDate(2026,5,5), .otherExpense = 10};
	request::Consumption cReq7{ .personInput = p1ID, .date = QDate::currentDate(), .otherExpense = 10};
	request::Consumption cReq8{ .personInput = p2ID, .date = QDate(2024,10,10), .otherExpense = 10};

	// 6xEarning (>=2026 --> 1,2,9,10, total: 40), 4xSpending (>=2026 --> 3,4,5,6, total: 40), p1ID --> credit: 30
	request::Balance bReq1{ .type = BalanceType::Earning,  .description = "Eintrag 1", .amount = 10, .date = QDate::currentDate(), .comment = "Kommentar 1", .coveringpersonEntryID = std::nullopt };
	request::Balance bReq2{ .type = BalanceType::Earning,  .description = "Eintrag 2", .amount = 10, .date = QDate::currentDate(), .comment = "Kommentar 2", .coveringpersonEntryID = std::nullopt };
	request::Balance bReq3{ .type = BalanceType::Spending, .description = "Eintrag 3", .amount = 10, .date = QDate::currentDate(), .comment = "Kommentar 3", .coveringpersonEntryID = p1ID };
	request::Balance bReq4{ .type = BalanceType::Spending, .description = "Eintrag 4", .amount = 10, .date = QDate::currentDate(), .comment = "Kommentar 4", .coveringpersonEntryID = p1ID };
	request::Balance bReq5{ .type = BalanceType::Spending, .description = "Eintrag 5", .amount = 10, .date = QDate::currentDate(), .comment = "Kommentar 5", .coveringpersonEntryID = p1ID };
	request::Balance bReq6{ .type = BalanceType::Spending, .description = "Eintrag 6", .amount = 10, .date = QDate::currentDate(), .comment = "Kommentar 6", .coveringpersonEntryID = std::nullopt };
	request::Balance bReq7{ .type = BalanceType::Earning, .description = "Eintrag 7", .amount = 10, .date = QDate(2025,12,31), .comment = "Kommentar 7", .coveringpersonEntryID = std::nullopt};
	request::Balance bReq8{ .type = BalanceType::Earning, .description = "Eintrag 8", .amount = 10, .date = QDate(2025,12,31), .comment = "Kommentar 8", .coveringpersonEntryID = std::nullopt};
	request::Balance bReq9{ .type = BalanceType::Earning, .description = "Eintrag 9", .amount = 10, .date = QDate(2026,1,1), .comment = "Kommentar 9", .coveringpersonEntryID = std::nullopt};
	request::Balance bReq10{ .type = BalanceType::Earning, .description = "Eintrag 10", .amount = 10, .date = QDate(2026,1,2), .comment = "Kommentar 10", .coveringpersonEntryID = std::nullopt};

	// p1ID --> total: 15, toCredit: 5, toTip: 10, >=2026: 8
	request::Payment pEntry1{ .personEntryID = p1ID, .date = QDate::currentDate(), .amount = 1, .overpaymentType = OverpaymentDisposition::Credit };
	request::Payment pEntry2{ .personEntryID = p1ID, .date = QDate(2026,1,1), .amount = 1, .overpaymentType = OverpaymentDisposition::Credit};
	request::Payment pEntry3{ .personEntryID = p1ID, .date = QDate(2026,1,2), .amount = 1, .overpaymentType = OverpaymentDisposition::Credit};
	request::Payment pEntry4{ .personEntryID = p1ID, .date = QDate(2025,12,12), .amount = 2, .overpaymentType = OverpaymentDisposition::Credit};
	request::Payment pEntry5{ .personEntryID = p1ID, .date = QDate(2026,2,2), .amount = 5, .overpaymentType = OverpaymentDisposition::Tip};
	request::Payment pEntry6{ .personEntryID = p1ID, .date = QDate(2025,8,8), .amount = 5, .overpaymentType = OverpaymentDisposition::Tip};

	// total: 30
	request::ShareSettlement sEntry1{ .amount = 10 };
	request::ShareSettlement sEntry2{ .amount = 10 };
	request::ShareSettlement sEntry3{ .amount = 10 };

	qDebug() << "Adding consumption";
	coSer.addConsumption(cReq1);
	coSer.addConsumption(cReq2);
	coSer.addConsumption(cReq3);
	coSer.addConsumption(cReq4);
	coSer.addConsumption(cReq5);
	coSer.addConsumption(cReq6);
	coSer.addConsumption(cReq7);
	coSer.addConsumption(cReq8);
	qDebug() << "\n";

	qDebug() << "Adding balance items";
	baSer.addBalanceItem(bReq1);
	baSer.addBalanceItem(bReq2);
	baSer.addBalanceItem(bReq3);
	baSer.addBalanceItem(bReq4);
	baSer.addBalanceItem(bReq5);
	baSer.addBalanceItem(bReq6);
	baSer.addBalanceItem(bReq7);
	baSer.addBalanceItem(bReq8);
	baSer.addBalanceItem(bReq9);
	baSer.addBalanceItem(bReq10);
	qDebug() << "\n";

	qDebug() << "Adding payments";
	paSer.addPayment(pEntry1);
	paSer.addPayment(pEntry2);
	paSer.addPayment(pEntry3);
	paSer.addPayment(pEntry4);
	paSer.addPayment(pEntry5);
	paSer.addPayment(pEntry6);
	qDebug() << "\n";

	qDebug() << "Adding share settlements";
	baSer.addShareSettlement(sEntry1);
	baSer.addShareSettlement(sEntry2);
	baSer.addShareSettlement(sEntry3);
	qDebug() << "\n";

	// start UI
	qDebug() << "Starting UI";
	CashRegisterSystemUI sysUI(serviceBundle, repoBundle);
	sysUI.show();

	return app.exec();
}

/*
* Ideas:
* - class PendingChangeLog with entries PendingChanges for change tracking before saving/syncing
*	- new button: sync -> save saves to local copy of database, sync pushes it to remote
*	- apply (save) button only active when changes were made
*	- "changes" window to show applied changes for inspection before saving
* - add basic calculator in balanceTab for cash counting
* - add buttons in PayTab: "Redeem All Credits", "Export Debts (CSV/CopyToClipboard/URL)"
* - add info-icons in BalanceTab (clickable/tooltip) -> get details on report calculation
* - add new tab for manual database changes (dropDown for which repository + QTableView)
*/ 