#include "SqliteDatabase.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <vector>
#include <optional>

namespace sqliteDatabase
{
	namespace
	{
		struct TableDef { QString name; QString body; };

		const std::vector<TableDef>& tableDefinitions()
		{
			static const std::vector<TableDef> defs = {
				{ "Person",
					"ID INTEGER PRIMARY KEY, firstName TEXT NOT NULL, lastName TEXT NOT NULL, "
					"nickName TEXT, info TEXT" },
				{ "Balance",
					"ID INTEGER PRIMARY KEY, type INTEGER NOT NULL, description TEXT, amount REAL NOT NULL, "
					"dateBooked TEXT, dateBookedSpecial INTEGER, dateAdded TEXT NOT NULL, comment TEXT, "
					"personID INTEGER REFERENCES Person(ID)" },
				{ "Debt",
					"ID INTEGER PRIMARY KEY, personID INTEGER NOT NULL REFERENCES Person(ID), "
					"dateBooked TEXT, dateBookedSpecial INTEGER, dateAdded TEXT NOT NULL, "
					"amount REAL NOT NULL, foreignShare REAL" },
				{ "Consumption",
					"ID INTEGER PRIMARY KEY, debtID INTEGER NOT NULL REFERENCES Debt(ID), "
					"nBeer05 INTEGER NOT NULL DEFAULT 0, nBeer04 INTEGER NOT NULL DEFAULT 0, "
					"nSoftdrinks INTEGER NOT NULL DEFAULT 0, nWater INTEGER NOT NULL DEFAULT 0, "
					"otherExpense REAL NOT NULL DEFAULT 0" },
				{ "Credit",
					"ID INTEGER PRIMARY KEY, personID INTEGER NOT NULL REFERENCES Person(ID), "
					"dateBooked TEXT, dateBookedSpecial INTEGER, dateAdded TEXT NOT NULL, "
					"amount REAL NOT NULL, description TEXT" },
				{ "Payment",
					"ID INTEGER PRIMARY KEY, personID INTEGER NOT NULL REFERENCES Person(ID), "
					"dateBooked TEXT, dateBookedSpecial INTEGER, dateAdded TEXT NOT NULL, "
					"amount REAL NOT NULL, overpaymentType INTEGER NOT NULL" },
				{ "PaymentAllocation",
					"ID INTEGER PRIMARY KEY, debtID INTEGER NOT NULL REFERENCES Debt(ID), "
					"paymentID INTEGER NOT NULL REFERENCES Payment(ID), amount REAL NOT NULL" },
				{ "ShareSettlement",
					"ID INTEGER PRIMARY KEY, dateBooked TEXT, dateBookedSpecial INTEGER, "
					"dateAdded TEXT NOT NULL, amount REAL NOT NULL" },
				{ "ShareSettlementAllocation",
					"ID INTEGER PRIMARY KEY, debtID INTEGER NOT NULL REFERENCES Debt(ID), "
					"shareSettlementID INTEGER NOT NULL REFERENCES ShareSettlement(ID), amount REAL NOT NULL" },
			};
			return defs;
		}

		/////////////////// TABLE INITIALIZATION, IF BLANK /////////////////////////////////////

		QString createTableSql(const QString& name, const QString& body)
		{
			return QString("CREATE TABLE %1 (%2) STRICT").arg(name, body);
		}

		void initBlankDatabase(QSqlDatabase& db)
		{
			db.transaction();

			QSqlQuery query(db);
			for (const auto& def : tableDefinitions())
			{
				if (!query.exec(createTableSql(def.name, def.body)))
					qDebug() << "Database initialization failed: " << query.lastError().text();
			}

			db.commit();
		}

		////////////////// TABLE STRUCTURE CHECK ON OPEN ///////////////////////////////////////

		QMap<QString, QStringList> describeSchema(QSqlDatabase& db)
		{
			struct TableInfo { QString name; int strict; };
			std::vector<TableInfo> tables;
			{
				QSqlQuery q(db);
				q.exec("PRAGMA table_list");
				while (q.next())
				{
					const QString name = q.value("name").toString();
					if (q.value("schema").toString() != "main" || q.value("type").toString() != "table"
						|| name.startsWith("sqlite_"))
						continue;
					tables.push_back({ name, q.value("strict").toInt() });
				}
			}

			QMap<QString, QStringList> schema;
			for (const auto& t : tables)
			{
				QStringList lines;
				lines << QString("strict=%1").arg(t.strict);

				QSqlQuery cols(db);
				cols.exec(QString("PRAGMA table_info(%1)").arg(t.name));
				while (cols.next())
					lines << QString("column %1 %2 notnull=%3 default=%4 pk=%5")
					.arg(cols.value("name").toString(), cols.value("type").toString(),
						cols.value("notnull").toString(), cols.value("dflt_value").toString(),
						cols.value("pk").toString());

				QStringList foreignKeys;   
				QSqlQuery fk(db);
				fk.exec(QString("PRAGMA foreign_key_list(%1)").arg(t.name));
				while (fk.next())
					foreignKeys << QString("foreign key %1 -> %2.%3")
					.arg(fk.value("from").toString(), fk.value("table").toString(), fk.value("to").toString());
				foreignKeys.sort();
				lines << foreignKeys;

				schema.insert(t.name, lines);
			}
			return schema;
		}

		// throwaway in-memory database
		std::optional<QMap<QString, QStringList>> expectedSchema()
		{
			const QString connection = "schema_reference";
			std::optional<QMap<QString, QStringList>> result;
			{
				QSqlDatabase ref = QSqlDatabase::addDatabase("QSQLITE", connection);
				ref.setDatabaseName(":memory:");
				if (ref.open())
				{
					bool ok = true;
					{
						QSqlQuery q(ref);
						for (const auto& def : tableDefinitions())
							ok = q.exec(createTableSql(def.name, def.body)) && ok;
					}
					if (ok) result = describeSchema(ref);
					ref.close();
				}
			}  
			QSqlDatabase::removeDatabase(connection);
			return result;
		}

		QStringList schemaDifferences(const QMap<QString, QStringList>& actual,
			const QMap<QString, QStringList>& expected)
		{
			QStringList diffs;

			for (auto it = expected.cbegin(); it != expected.cend(); ++it)
			{
				const QString& table = it.key();
				if (!actual.contains(table)) { diffs << QString("missing table %1").arg(table); continue; }

				const QStringList& e = it.value();
				const QStringList& a = actual[table];
				const qsizetype before = diffs.size();

				for (const QString& line : e)
					if (!a.contains(line)) diffs << QString("%1: missing '%2'").arg(table, line);
				for (const QString& line : a)
					if (!e.contains(line)) diffs << QString("%1: unexpected '%2'").arg(table, line);
				if (diffs.size() == before && a != e)
					diffs << QString("%1: column order differs").arg(table);
			}
			for (auto it = actual.cbegin(); it != actual.cend(); ++it)
				if (!expected.contains(it.key())) diffs << QString("unexpected table %1").arg(it.key());

			return diffs;
		}

		bool verifySchema(QSqlDatabase& db)
		{
			const auto expected = expectedSchema();
			if (!expected)
			{
				qWarning() << "Could not build the reference schema for verification!";
				return false;
			}

			const QStringList diffs = schemaDifferences(describeSchema(db), *expected);
			if (diffs.isEmpty()) return true;

			qWarning() << "Database structure differs from the defined one:";
			for (const QString& d : diffs) qWarning().noquote() << "  -" << d;
			return false;
		}

		////////////////// FROM HERE: MIGRATION, DELETE FUNCTIONS AFTER ONE TIME USE ///////////

		const QStringList kDatedTables{ "Balance", "Debt", "Credit", "Payment", "ShareSettlement" };
		const QString kFallbackDate = "'2026-09-28'"; 

		QStringList columnsOf(QSqlDatabase& db, const QString& table)
		{
			QStringList cols;
			QSqlQuery q(db);
			if (q.exec(QString("PRAGMA table_info(%1)").arg(table)))
				while (q.next()) cols << q.value("name").toString();
			return cols; 
		}

		bool needsMigration(const QStringList& cols)
		{
			return !cols.isEmpty() && (!cols.contains("dateAdded") || !cols.contains("dateBookedSpecial"));
		}

		QString sourceExpression(const QString& col, const QStringList& oldCols)
		{
			if (oldCols.contains(col))                      
				return col;                                

			if (col == "dateBookedSpecial")
				return "NULL";

			const bool hadLegacyDate = oldCols.contains("date");
			if (col == "dateBooked")
				return hadLegacyDate ? "date" : kFallbackDate;
			if (col == "dateAdded")
				return hadLegacyDate ? "COALESCE(date, " + kFallbackDate + ")" : kFallbackDate;

			return col;   
		}

		bool rebuildTable(QSqlDatabase& db, const TableDef& def)
		{
			QSqlQuery q(db);
			const auto run = [&](const QString& sql)
				{
					if (q.exec(sql)) return true;
					qWarning() << "Migration of" << def.name << "failed:" << q.lastError().text() << "| SQL:" << sql;
					return false;
				};

			const QString newName = def.name + "_new";
			const QStringList oldCols = columnsOf(db, def.name);

			if (!run(createTableSql(newName, def.body))) return false;

			const QStringList newCols = columnsOf(db, newName);
			QStringList selectExprs;
			for (const QString& c : newCols)
				selectExprs << sourceExpression(c, oldCols);

			return run(QString("INSERT INTO %1 (%2) SELECT %3 FROM %4")
				.arg(newName, newCols.join(", "), selectExprs.join(", "), def.name))
				&& run(QString("DROP TABLE %1").arg(def.name))
				&& run(QString("ALTER TABLE %1 RENAME TO %2").arg(newName, def.name));
		}

		bool migrateDatedTables(QSqlDatabase& db)
		{
			std::vector<const TableDef*> pending;
			for (const auto& def : tableDefinitions())
				if (kDatedTables.contains(def.name) && needsMigration(columnsOf(db, def.name)))
					pending.push_back(&def);

			if (pending.empty()) return true;   

			const QString backupPath = db.databaseName() + "."
				+ QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss") + ".bak";
			if (!QFile::copy(db.databaseName(), backupPath))
			{
				qWarning() << "Could not create backup" << backupPath << "- migration aborted";
				return false;
			}

			QSqlQuery q(db);
			q.exec("PRAGMA foreign_keys");
			const bool fkWasOn = q.next() && q.value(0).toBool();
			q.exec("PRAGMA foreign_keys = OFF");

			db.transaction();
			bool ok = true;
			for (const TableDef* def : pending)
			{
				ok = rebuildTable(db, *def);
				if (!ok) break;
			}
			if (ok) ok = db.commit();
			else db.rollback();

			if (fkWasOn) q.exec("PRAGMA foreign_keys = ON");
			return ok;
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

		if (!dbExists)
		{
			initBlankDatabase(db);
		}
		else if (!migrateDatedTables(db))
		{
			qWarning() << "Database migration failed!";
			db.close();
			return false;
		}

		if (!verifySchema(db))
		{
			qWarning() << "Database structure check failed - database not compatible!";
			db.close();
			return false;
		}

		db.transaction(); // start transaction at program start. commit/rollback on save/discard

		return true;
	}
}