#include "gui/CashRegisterSystemUI.h"
#include "gui/types/GuiTypes.h"

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/model/DomainTypes.h"

#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include "domain/services/BalanceService.h"
#include "domain/services/PersonService.h"

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
	qDebug() << "-Reading config-";
	const PriceList priceList = priceListLoader::read();
	const registerFinancials::State financialStateBefore = financialStateLoader::read();
	qDebug() << "";

	// open database
	qDebug() << "-Opening database-";
	OneDriveSyncManager syncManager{};
	syncManager.setup();
	qDebug() << "Local database: " << syncManager.getLocalDatabasePath();
	qDebug() << "Remote database: " << syncManager.getRemoteDatabasePath();
	std::string dbPath = syncManager.getLocalDatabasePath();
	SqliteDatabase sqliteDB{};
	sqliteDB.open(dbPath);
	QSqlDatabase& db = sqliteDB.getDatabase();
	qDebug() << "";

	// create session control and changelog
	PendingChangeLog log{};
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

	BalanceService			baSer(repoBundle, financialStateBefore, log);
	PaymentService			paSer(repoBundle, log);
	PersonService			peSer(repoBundle, log);
	ConsumptionService		coSer(peSer, repoBundle, priceList, log);
	ServiceBundle serviceBundle{ .consumptionService = coSer, .paymentService = paSer, .balanceService = baSer, .personService = peSer };

	// domain testing
	//testing::addTestEntries(repoBundle, serviceBundle);

	// start UI
	qDebug() << "-Starting up GUI-";
	CashRegisterSystemUI sysUI(serviceBundle, repoBundle, controller, db, log);
	sysUI.show();
	qDebug() << "";

	qDebug() << "-Starting app execution-";
	int returnValue = app.exec();
	qDebug() << "";

	return returnValue;
}

/*
* Ideas:
* - general architecture:
*		- predefining cash, foreign cash, savings in state before, previous debts and credits all at the same time is redundant
*				-> cash at specific time stamp would be enough as database entries from before arent cleared
*				-> allow cash amount validation at any time, which either: refreshes state before, or, better: create new database saving the validation checks
*				-> balance statistics are still displayed from beginning of year (if avaliable) to today
*				-> currently disabled special date, as "unknown" should be counted as after date of state before -> cleanup after architecture change
*		- remove consumption earnings from earnings table, display them in seperate section with more information
*/ 

/*
* Long-Term Goals
* - web-API: get debt per person
* - database API with Postgres and Raspberry PI 
* - port to Android
*/