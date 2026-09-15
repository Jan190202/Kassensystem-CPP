#include "SqliteDebtRepository.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteDebtRepository::addDebtEntry(const entry::Debt& entry)
{
	QSqlQuery query;
	query.prepare(
		"INSERT INTO Debt"
		"(personID, date, amount, foreignShare)"
		"VALUES (:personID, :date, :amount, :foreignShare)"
		"RETURNING ID"
	);
	query.bindValue(":personID", entry.personEntryID);
	query.bindValue(":date", entry.date.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":foreignShare", entry.foreignShare);
	
	qDebug() << entry;

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

double SqliteDebtRepository::getPersonsTotal(int64_t personEntryID) const
{
	QSqlQuery query;
	query.prepare(
		"SELECT SUM(amount) AS total "
		"FROM Debt "
		"WHERE Debt.personID = :personEntryID "
	);
	query.bindValue(":personEntryID", personEntryID);

	if (query.exec())
		if (query.next())
		{
			qDebug() << "Total: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}

double SqliteDebtRepository::getPersonsDue(int64_t personEntryID) const
{
	QSqlQuery query;
	query.prepare(
		"WITH TotalDebt AS ( "
			"SELECT COALESCE(SUM(Debt.amount), 0) AS debt "
			"FROM Debt "
			"WHERE Debt.personID = :personEntryID "
		"), "
		"TotalPaid AS ( "
		"	SELECT COALESCE(SUM(PaymentAllocation.amount), 0) AS paid "
		"	FROM PaymentAllocation "
		"	INNER JOIN Debt ON PaymentAllocation.debtID = Debt.ID "
		"	WHERE Debt.personID = :personEntryID "
		") "
		"SELECT TotalDebt.debt - TotalPaid.paid AS due "
		"FROM TotalDebt, TotalPaid "
	);
	query.bindValue(":personEntryID", personEntryID);

	if (query.exec())
		if (query.next())
		{
			qDebug() << "Due: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}

double SqliteDebtRepository::getPersonsPaid(int64_t personEntryID) const
{
	QSqlQuery query;
	query.prepare(
		"SELECT COALESCE(SUM(PaymentAllocation.amount), 0) AS paid "
		"FROM PaymentAllocation "
		"INNER JOIN Debt ON PaymentAllocation.debtID = Debt.ID "
		"WHERE Debt.personID = :personEntryID "
	);
	query.bindValue(":personEntryID", personEntryID);

	if (query.exec())
		if (query.next())
		{
			qDebug() << "Paid: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}

double SqliteDebtRepository::getTotalShare(FinancialShare share, const QDate& minDate) const
{
	QString shareString;
	switch (share)
	{
	case FinancialShare::All:
		shareString = "1";
		break;
	case FinancialShare::Own:
		shareString = "(1-Debt.foreignShare)";
		break;
	case FinancialShare::Foreign:
		shareString = "Debt.foreignShare";
		break;
	}
	
	QSqlQuery query;
	query.prepare(
		"SELECT SUM(Debt.amount * " + shareString + ") AS share "
		"FROM Debt "
		"WHERE Debt.date >= :date "
	);
	query.bindValue(":date", minDate.toString(Qt::ISODate));

	if (query.exec())
		
		if (query.next())
		{
			qDebug() << "Share: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}

double SqliteDebtRepository::getForeignDue() const
{
	QSqlQuery query;
	query.prepare(
		"WITH SettledShares AS ( "
			"SELECT SUM(ShareSettlement.amount) as total "
			"FROM ShareSettlement "
		") "
		"SELECT SUM(Debt.amount * Debt.foreignShare) - SettledShares.total AS foreignDue "
		"FROM Debt "
	);

	if (query.exec())
		if (query.next())
		{
			qDebug() << "ForeignDue: " << query.value(0).toDouble();;
			return query.value(0).toDouble();
		}
}

std::vector<entry::Outstanding> SqliteDebtRepository::getPersonsOutstandingEntries(int64_t personEntryID, FilterType type) const
{
	QString sqlString;

	if (type == FilterType::OmitFullyPaid) sqlString = " AND (Debt.amount - COALESCE(GroupedPayments.paid,0)) > 0";
	
	QSqlQuery query;
	query.prepare(
		"WITH GroupedPayments AS ( "
			"SELECT PaymentAllocation.debtID AS debtID, SUM(PaymentAllocation.amount) AS paid "
			"FROM PaymentAllocation "
			"GROUP BY PaymentAllocation.debtID "
		") "
		"SELECT Debt.ID, Debt.date, Debt.amount, Debt.amount - COALESCE(GroupedPayments.paid,0) AS remaining "
		"FROM Debt "
		"LEFT OUTER JOIN GroupedPayments ON Debt.ID = GroupedPayments.debtID "
		"WHERE Debt.personID = :personEntryID" + sqlString
	);
	query.bindValue(":personEntryID", personEntryID);

	std::vector<entry::Outstanding> entries;

	if (query.exec())
	{
		while (query.next())
		{
			entries.emplace_back(getOutstandingEntryFromQuery(query));
		}
	}

	qDebug() << "Person Outstanding: ";
	for (auto& entry : entries)
		qDebug() << entry;

	return entries;
}

std::vector<entry::Outstanding> SqliteDebtRepository::getForeignShareOutstandingEntries(FilterType type) const
{
	QString sqlString;

	if (type == FilterType::OmitFullyPaid) sqlString = " WHERE Debt.amount - COALESCE(GroupedShareSettlements.settled,0) > 0";

	QSqlQuery query;
	query.prepare(
		"WITH GroupedShareSettlements AS ( "
		"SELECT ShareSettlementAllocation.debtID AS debtID, SUM(ShareSettlementAllocation.amount) AS settled "
		"FROM ShareSettlementAllocation "
		"GROUP BY ShareSettlementAllocation.debtID "
		") "
		"SELECT Debt.ID, Debt.date, Debt.amount, Debt.amount - COALESCE(GroupedShareSettlements.settled,0) AS remaining "
		"FROM Debt "
		"LEFT OUTER JOIN GroupedShareSettlements ON Debt.ID = GroupedShareSettlements.debtID "
		+ sqlString
	);

	std::vector<entry::Outstanding> entries;

	if (query.exec())
	{
		while (query.next())
		{
			entries.emplace_back(getOutstandingEntryFromQuery(query));
		}
	}

	qDebug() << "Foreign Outstanding: ";
	for (auto& entry : entries)
		qDebug() << entry;

	return entries;
}

entry::Outstanding SqliteDebtRepository::getOutstandingEntryFromQuery(const QSqlQuery& query) const
{
	return entry::Outstanding{
		.debtEntryID = query.value("ID").toLongLong(),
		.date = query.value("date").toDate(),
		.amount = query.value("amount").toDouble(),
		.remaining = query.value("remaining").toDouble()
	};
}