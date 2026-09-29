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
	QSqlQuery query;
	query.prepare(
		"INSERT INTO Credit "
		"(personID, dateBooked, dateBookedSpecial, dateAdded, amount, description) "
		"VALUES (:personID, :dateBooked, :dateBookedSpecial, :dateAdded, :amount, :description) "
		"RETURNING ID"
	);
	query.bindValue(":personID", entry.personEntryID);
	query.bindValue(":dateBooked", entry.dateBooked.toSqlDateValue());
	query.bindValue(":dateBookedSpecial", entry.dateBooked.toSqlSpecialValue());
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":description", QString::fromStdString(entry.description));

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

double SqliteCreditRepository::getPersonsCredit(int64_t personEntryID) const
{
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
			qDebug() << "Persons Credit: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}

double SqliteCreditRepository::getTotalDepositedCredit(const QDate& minDate) const
{
	QSqlQuery query;

	QString compClause = sqliteUtils::registerDateCompareClause(
		sqliteUtils::Op::largerOrEq,
		RegisterDate{ minDate },
		"dateBooked",
		"dateBookedSpecial",
		":minDate");

	query.prepare(
		"SELECT COALESCE(SUM(amount),0) "
		"FROM Credit "
		"WHERE " + compClause + " "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	if (query.exec())
		if (query.next())
		{
			qDebug() << "Total Desposited Credit: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}