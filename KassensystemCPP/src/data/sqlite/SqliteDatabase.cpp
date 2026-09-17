#include "SqliteDatabase.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <vector>

namespace sqliteDatabase
{
	namespace
	{
		void initBlankDatabase(QSqlDatabase& db)
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

			const int nTables = 9;
			std::vector<QString> creationQueries;
			creationQueries.reserve(nTables);

			creationQueries.emplace_back(
				"CREATE TABLE Person ("
				"ID			INTEGER PRIMARY KEY,"
				"firstName	TEXT NOT NULL,"
				"lastName	TEXT NOT NULL,"
				"nickName	TEXT,"
				"info		TEXT"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE Balance ("
				"ID				INTEGER PRIMARY KEY,"
				"type			INTEGER NOT NULL,"
				"description	TEXT,"
				"amount			REAL NOT NULL,"
				"dateBooked		TEXT NOT NULL,"
				"dateAdded		TEXT NOT NULL,"
				"comment		TEXT,"
				"personID		INTEGER REFERENCES Person(ID)"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE Debt ("
				"ID				INTEGER PRIMARY KEY,"
				"personID		INTEGER NOT NULL REFERENCES Person(ID),"
				"date			TEXT NOT NULL,"
				"amount			REAL NOT NULL,"
				"foreignShare	REAL"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE Consumption ("
				"ID					INTEGER PRIMARY KEY,"
				"debtID				INTEGER NOT NULL REFERENCES Debt(ID),"
				"nBeer05			INTEGER NOT NULL DEFAULT 0,"
				"nBeer04			INTEGER NOT NULL DEFAULT 0,"
				"nSoftdrinks		INTEGER NOT NULL DEFAULT 0,"
				"nWater				INTEGER NOT NULL DEFAULT 0,"
				"otherExpense		REAL NOT NULL DEFAULT 0"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE Credit ("
				"ID				INTEGER PRIMARY KEY,"
				"personID		INTEGER NOT NULL REFERENCES Person(ID),"
				"date			TEXT NOT NULL,"
				"amount			REAL NOT NULL,"
				"description	TEXT"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE Payment ("
				"ID					INTEGER PRIMARY KEY,"
				"personID			INTEGER NOT NULL REFERENCES Person(ID),"
				"date				TEXT NOT NULL,"
				"amount				REAL NOT NULL,"
				"overpaymentType	INTEGER NOT NULL"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE PaymentAllocation ("
				"ID			INTEGER PRIMARY KEY,"
				"debtID		INTEGER NOT NULL REFERENCES Debt(ID),"
				"paymentID	INTEGER NOT NULL REFERENCES Payment(ID),"
				"amount		REAL NOT NULL"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE ShareSettlement ("
				"ID		INTEGER PRIMARY KEY,"
				"date	TEXT NOT NULL,"
				"amount	REAL NOT NULL"
				") STRICT"
			);

			creationQueries.emplace_back(
				"CREATE TABLE ShareSettlementAllocation ("
				"ID					INTEGER PRIMARY KEY,"
				"debtID				INTEGER NOT NULL REFERENCES Debt(ID),"
				"shareSettlementID	INTEGER NOT NULL REFERENCES ShareSettlement(ID),"
				"amount				REAL NOT NULL"
				") STRICT"
			);

			db.transaction();

			QSqlQuery query;
			for (const auto& queryString : creationQueries)
			{
				if (!query.exec(queryString)) qDebug() << "Database initialization failed: " << query.lastError().text();
			}

			db.commit();
		}
	}

	bool open(const std::string& dbPathStr)
	{
		QString dbPath = QString::fromStdString(dbPathStr);

		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

		bool dbExists = QFile::exists(dbPath);
		if (!dbExists) qWarning() << "Specified path for database doesn't exist. Creating blank database!";

		db.setDatabaseName(dbPath); // or ":memory:" for in-memory database

		bool dbSuccessfullyOpened = db.open(); // opens blank database if !dbExists
		if (!dbSuccessfullyOpened)
		{
			qWarning() << "Opening database failed!";
			return false;
		}

		if (!dbExists) initBlankDatabase(db);

		db.transaction(); // start transaction at program start. commit/rollback on save/discard

		return true;
	}
}


