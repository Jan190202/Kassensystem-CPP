#include "SqliteDebtRepository.h"
#include "data/sqlite/sqliteutils/SqliteUtils.h"
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
		"(personID, dateBooked, dateBookedSpecial, dateAdded, amount, foreignShare)"
		"VALUES (:personID, :dateBooked, :dateBookedSpecial, :dateAdded, :amount, :foreignShare)"
		"RETURNING ID"
	);
	query.bindValue(":personID", entry.personEntryID);
	query.bindValue(":dateBooked", entry.dateBooked.toSqlDateValue());
	query.bindValue(":dateBookedSpecial", entry.dateBooked.toSqlSpecialValue());
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
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
		"SELECT COALESCE(SUM(amount), 0) AS total "
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

	QString compClause = sqliteUtils::registerDateCompareClause(
		sqliteUtils::Op::largerOrEq,
		RegisterDate{ minDate },
		"Debt.dateBooked",
		"Debt.dateBookedSpecial",
		":minDate");
	
	QSqlQuery query;
	query.prepare(
		"SELECT COALESCE(SUM(Debt.amount * " + shareString + "),0) AS share "
		"FROM Debt "
		"WHERE " + compClause + " "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	qDebug() << "in getTotalShare: " << "SELECT COALESCE(SUM(Debt.amount * " + shareString + "),0) AS share "
		"FROM Debt "
		"WHERE " + compClause + " ";

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
			"SELECT COALESCE(SUM(ShareSettlement.amount),0) as total "
			"FROM ShareSettlement "
		") "
		"SELECT ROUND(COALESCE(SUM(Debt.amount * Debt.foreignShare),0) - SettledShares.total,3) AS foreignDue "
		"FROM Debt "
		"CROSS JOIN SettledShares"
	);

	if (query.exec())
		if (query.next())
		{
			qDebug() << "ForeignDue: " << query.value(0).toDouble();;
			return query.value(0).toDouble();
		}
	qWarning() << "Query failed:" << query.lastError().text();
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
		"SELECT Debt.ID, Debt.dateBooked, Debt.dateBookedSpecial, Debt.amount, Debt.amount - COALESCE(GroupedPayments.paid,0) AS remaining "
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
	QString filter;
	if (type == FilterType::OmitFullyPaid)
		filter = " AND ROUND(Debt.amount * Debt.foreignShare - COALESCE(GroupedShareSettlements.settled, 0), 2) > 0";

	QSqlQuery query;
	query.prepare(
		"WITH GroupedShareSettlements AS ( "
			"SELECT ShareSettlementAllocation.debtID AS debtID, SUM(ShareSettlementAllocation.amount) AS settled "
			"FROM ShareSettlementAllocation "
			"GROUP BY ShareSettlementAllocation.debtID "
		") "
		"SELECT "
			"Debt.ID, Debt.dateBooked, Debt.dateBookedSpecial, "
			"Debt.amount * Debt.foreignShare AS amount, "
			"Debt.amount * Debt.foreignShare - COALESCE(GroupedShareSettlements.settled, 0) AS remaining "
		"FROM Debt "
		"LEFT OUTER JOIN GroupedShareSettlements ON Debt.ID = GroupedShareSettlements.debtID "
		"WHERE Debt.foreignShare > 0" + filter
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
		.dateBooked = 
			!query.value("dateBooked").isNull() ? 
				RegisterDate{query.value("dateBooked").toDate()} : 
				RegisterDate{static_cast<RegisterDate::Special>(query.value("dateBookedSpecial").toInt())},
		.amount = query.value("amount").toDouble(),
		.remaining = query.value("remaining").toDouble()
	};
}