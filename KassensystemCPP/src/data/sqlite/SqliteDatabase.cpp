#include "SqliteDatabase.h"
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <vector>

bool SqliteDatabase::openDatabase(const std::string& dbPathStr)
{
	QString dbPath = QString::fromStdString(dbPathStr);

	QSqlDatabase db = QSqlDatabase::addDatabase("SQLITE");
	
	bool dbExists = QFile::exists(dbPath);
	if (!dbExists) qWarning() << "Specified path for database doesn't exist. Creating blank database!";
	
	db.addDatabase(":memory:"); // dbPath later
	
	bool dbSuccessfullyOpened = db.open(); // opens blank database if !dbExists
	if (!dbSuccessfullyOpened)
	{
		qWarning() << "Opening database failed!";
		return false;
	}

	if (!dbExists) initBlankDatabase(db);

	db.transaction(); // start transaction at program start. commit/rollback on save/discard
}

void SqliteDatabase::commitChanges()
{
	QSqlDatabase::database().commit();
}

void SqliteDatabase::rollbackChanges()
{
	QSqlDatabase::database().rollback();
}

void SqliteDatabase::initBlankDatabase(QSqlDatabase& db)
{
	/*
	* Setup tables for blank database:
	*	- Balance					(ID, balanceType, description, amount, dateBooked, dateAdded, comment personID)
	*	- Consumption				(ID, debtID, nBeer05, nBeer04, nSoftdrinks, nWater, otherExpense)
	*	- Credit					(ID, personID, date, amount, description)
	*	- Debt						(ID, personID, date, amount, foreignShare)
	*	- Payment					(ID, personID, date, amount, overpaymentType)
	*	- PaymentAllocation			(ID, debtID, paymentID, amount)
	*	- Person					(ID, firstName, lastName, nickName, info)
	*	- ShareSettlement			(ID, date, amount)
	*	- ShareSettlementAllocation (ID, debtID, shareSettlementID, amount)
	*/

	/*
		CREATE TABLE Employees (
			emp_id     INTEGER PRIMARY KEY,
			first_name TEXT NOT NULL,
			salary     REAL,
			is_active  INTEGER,   -- no BOOLEAN keyword in STRICT mode; use INTEGER 0/1
			resume     BLOB
		) STRICT;
	*/ 

	const int nTables = 9;
	std::vector<QString> creationQueries;
	creationQueries.reserve(nTables);

	creationQueries.emplace_back(
		"CREATE ..."
		"..."
	);


	db.transaction();

	QSqlQuery query;
	for (const auto& queryString : creationQueries)
	{
		if (!query.exec(queryString)) qDebug() << "Database initialization failed: " << query.lastError().text();
	}

	db.commit();
}
