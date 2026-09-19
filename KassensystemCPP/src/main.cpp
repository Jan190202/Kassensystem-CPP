#include "gui/CashRegisterSystemUI.h"
#include "gui/GuiTypes.h"

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/model/DomainTypes.h"

#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include "domain/services/BalanceService.h"

#include "data/sqlite/repositories/SqliteBalanceRepository.h";
#include "data/sqlite/repositories/SqliteConsumptionRepository.h";
#include "data/sqlite/repositories/SqliteCreditRepository.h";
#include "data/sqlite/repositories/SqliteDebtRepository.h";
#include "data/sqlite/repositories/SqlitePaymentRepository.h";
#include "data/sqlite/repositories/SqlitePersonRepository.h";
#include "data/sqlite/repositories/SqliteShareSettlementRepository.h";

#include "data/config/PriceListLoader.h"
#include "data/config/FinancialStateLoader.h"
#include "data/sqlite/SqliteDatabase.h"

#include "data/sqlite/storage/SyncManager.h"
#include "data/sqlite/storage/onedrivesync/OneDriveSyncManager.h"

#include "domain/SessionController.h"
#include "data/sqlite/sessioncontrol/SqliteSessionController.h"

#include "app/RepositoryBundle.h"
#include "app/ServiceBundle.h"

#include "system/SystemConfig.h"

#include <string>
#include <iostream>
#include <optional>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include "Testing.h"

int main(int argc, char* argv[])
{
	// configure system
	systemConfig::setUTF8Encoding();

	// initialize QApp
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("KassensystemSVU");

	// load read-only data
	qDebug() << "Reading config:";
	const PriceList priceList = priceListLoader::read();
	const registerFinancials::State financialStateBefore = financialStateLoader::read();
	qDebug() << "";

	// open database
	OneDriveSyncManager syncManager{};
	syncManager.setup();
	qDebug() << "Local database: " << syncManager.getLocalDatabasePath();
	qDebug() << "Remote database: " << syncManager.getRemoteDatabasePath();
	std::string dbPath = syncManager.getLocalDatabasePath();
	sqliteDatabase::open(dbPath);

	// create session control
	SqliteSessionController sqliteController{ syncManager };
	SessionController& controller = sqliteController;

	// initialize repositories and services
	PersonRepository* peRep				= new SqlitePersonRepository();
	ConsumptionRepository* coRep		= new SqliteConsumptionRepository();
	DebtRepository* deRep				= new SqliteDebtRepository();
	PaymentRepository* paRep			= new SqlitePaymentRepository();
	CreditRepository* crRep				= new SqliteCreditRepository();
	BalanceRepository* baRep			= new SqliteBalanceRepository();
	ShareSettlementRepository* seRep	= new SqliteShareSettlementRepository();
	RepositoryBundle repoBundle{ .personRepo = peRep, .consumptionRepo = coRep, .debtRepo = deRep, .paymentRepo = paRep, .creditRepo = crRep, .balanceRepo = baRep, .shareSettlementRepo = seRep };

	ConsumptionService		coSer(repoBundle, priceList);
	BalanceService			baSer(repoBundle, financialStateBefore);
	PaymentService			paSer(repoBundle);
	ServiceBundle serviceBundle{ .consumptionService = coSer, .paymentService = paSer, .balanceService = baSer };

	// domain testing
	//testing::addTestEntries(repoBundle, serviceBundle);

	// start UI
	qDebug() << "Starting UI\n";
	CashRegisterSystemUI sysUI(serviceBundle, repoBundle, controller);
	sysUI.show();
	int returnValue = app.exec();

	return returnValue;
}

/*
* Ideas:
* - class PendingChangeLog with entries PendingChanges for change tracking before saving/syncing
*	- new button: sync -> save saves to local copy of database, sync pushes it to remote
*	- apply (save) button only active when changes were made
*	- "changes" window to show applied changes for inspection before saving
* - BalanceTab:
*	- add basic calculator in balanceTab for cash counting
*	- add info-icons (clickable/tooltip) -> get details on report calculation
*		-> cashDiff (done)
*		-> explanation for difference between current cash and current savings
*	- remove consumption earnings from earnings table, display them in seperate section with more information
* - add buttons in PayTab: "Redeem All Credits", "Export Debts (CSV/CopyToClipboard/URL)"
* - add new tab for manual database changes (dropDown for which repository + QTableView)
* - web-API: get debt per person
*/ 