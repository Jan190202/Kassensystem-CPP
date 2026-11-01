#include "SqliteCreditRepository.h"
#include "data/sqlite/sqliteutils/SqliteUtils.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteCreditRepository::addCreditEntry(const entry::Credit& entry)
{
	QDebug deb = qDebug();
	deb << "-> addCreditEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO Credit "
		"(personID, dateBooked, dateBookedSpecial, dateAdded, amount, description) "
		"VALUES (:personID, :dateBooked, :dateBookedSpecial, :dateAdded, :amount, :description) "
		"RETURNING ID"
	);
	query.bindValue(":personID", entry.person.personEntryID);
	query.bindValue(":dateBooked", entry.dateBooked.toSqlDateValue());
	query.bindValue(":dateBookedSpecial", entry.dateBooked.toSqlSpecialValue());
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":description", QString::fromStdString(entry.description));

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}

	deb << "--> failed: " << query.lastError().text();
	return -1;
}

double SqliteCreditRepository::getPersonsCredit(int64_t personEntryID) const
{
	QDebug deb = qDebug();
	deb << "-> getPersonsCredit";

	QSqlQuery query;
	query.prepare(
		"SELECT COALESCE(SUM(amount),0) "
		"FROM Credit "
		"WHERE personID = :personEntryID "
	);
	query.bindValue(":personEntryID", personEntryID);

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

double SqliteCreditRepository::getTotalDepositedCredit(const DateRange& range) const
{
	QDebug deb = qDebug();
	deb << "-> getTotalDepositedCredit";

	QString rangeClause = sqliteUtils::bookedRangeClause(
		range,
		"Credit.dateBooked",
		"Credit.dateBookedSpecial",
		"Credit.dateAdded",
		"range");

	QSqlQuery query;
	query.prepare(
		"SELECT COALESCE(SUM(Credit.amount),0) "
		"FROM Credit "
		"WHERE " + rangeClause + " "
	);
	sqliteUtils::bindDateRange(query, range, "range");

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

double SqliteCreditRepository::getTotalUsedCredit(const DateRange& /*range*/) const
{
	// The database has no table that records credit being used up (offset against debts) yet,
	// so nothing is ever consumed. When such a table is added, sum it here by the day the using entry was added.
	return 0.0;
}

double SqliteCreditRepository::getOpenCredit(const DateRange& range) const
{
	// stock as of the upper bound: everything deposited up to then, minus what was used up to then
	const DateRange upTo = DateRange::upTo(range.untilInclusive);
	return getTotalDepositedCredit(upTo) - getTotalUsedCredit(upTo);
}