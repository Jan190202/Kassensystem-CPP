#include "SqliteCreditRepository.h"
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
		"(personID, date, amount, description) "
		"VALUES (:personID, :date, :amount, :description) "
		"RETURNING ID"
	);
	query.bindValue(":personID", entry.personEntryID);
	query.bindValue(":date", entry.date.toString(Qt::ISODate));
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
		"SELECT SUM(amount) "
		"FROM Credit "
		"WHERE ID = :personID "
	);
	query.bindValue(":personID", personEntryID);

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
	query.prepare(
		"SELECT SUM(amount) "
		"FROM Credit "
		"WHERE date >= :minDate "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	if (query.exec())
		if (query.next())
		{
			qDebug() << "Total Desposited Credit: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}