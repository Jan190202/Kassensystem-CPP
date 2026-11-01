#include "SqliteDebtRepository.h"
#include "data/sqlite/sqliteutils/SqliteUtils.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

namespace
{
	// stocks are "as of" a date: only the upper bound of the passed range counts
	QString debtBookedUpToClause(const DateRange& upTo)
	{
		return sqliteUtils::bookedRangeClause(upTo, "Debt.dateBooked", "Debt.dateBookedSpecial", "Debt.dateAdded", "asOf");
	}
}

int64_t SqliteDebtRepository::addDebtEntry(const entry::Debt& entry)
{
	QDebug deb = qDebug();
	deb << "-> addDebtEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO Debt "
		"(personID, dateBooked, dateBookedSpecial, dateAdded, amount, foreignShare) "
		"VALUES (:personID, :dateBooked, :dateBookedSpecial, :dateAdded, :amount, :foreignShare) "
		"RETURNING ID "
	);
	query.bindValue(":personID", entry.person.personEntryID);
	query.bindValue(":dateBooked", entry.dateBooked.toSqlDateValue());
	query.bindValue(":dateBookedSpecial", entry.dateBooked.toSqlSpecialValue());
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":foreignShare", entry.foreignShare);

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}

	deb << "--> failed: " << query.lastError().text();
	return -1;
}

double SqliteDebtRepository::getPersonsTotal(int64_t personEntryID) const
{
	QDebug deb = qDebug();
	deb << "-> getPersonsTotal";

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
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

double SqliteDebtRepository::getPersonsDue(int64_t personEntryID) const
{
	QDebug deb = qDebug();
	deb << "-> getPersonsDue";

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
			deb << "returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

double SqliteDebtRepository::getPersonsPaid(int64_t personEntryID) const
{
	QDebug deb = qDebug();
	deb << "-> getPersonsPaid";

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
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

double SqliteDebtRepository::getTotalShare(FinancialShare share, const DateRange& range) const
{
	QDebug deb = qDebug();
	deb << "-> getTotalShare";

	QString shareString;
	switch (share)
	{
	case FinancialShare::all:
		shareString = "1";
		break;
	case FinancialShare::own:
		shareString = "(1-Debt.foreignShare)";
		break;
	case FinancialShare::foreign:
		shareString = "Debt.foreignShare";
		break;
	}

	QString rangeClause = sqliteUtils::bookedRangeClause(
		range,
		"Debt.dateBooked",
		"Debt.dateBookedSpecial",
		"Debt.dateAdded",
		"range");

	QSqlQuery query;
	query.prepare(
		"SELECT COALESCE(SUM(Debt.amount * " + shareString + "),0) AS share "
		"FROM Debt "
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

double SqliteDebtRepository::getForeignDue() const
{
	return getForeignDue(DateRange{}); // right now: no upper bound
}

double SqliteDebtRepository::getForeignDue(const DateRange& range) const
{
	QDebug deb = qDebug();
	deb << "-> getForeignDue(range)";

	const DateRange upTo = DateRange::upTo(range.untilInclusive);
	const QString debtClause = debtBookedUpToClause(upTo);
	const QString settlementClause = sqliteUtils::addedRangeClause(upTo, "ShareSettlement.dateAdded", "asOf");

	// foreign share of all debts booked up to the date, minus settlement allocations (to those debts)
	// whose settlement was added up to the date
	QSqlQuery query;
	query.prepare(
		"SELECT ROUND( "
		"COALESCE((SELECT SUM(Debt.amount * Debt.foreignShare) FROM Debt WHERE " + debtClause + "), 0) - "
		"COALESCE(( "
		"SELECT SUM(ShareSettlementAllocation.amount) "
		"FROM ShareSettlementAllocation "
		"JOIN Debt ON ShareSettlementAllocation.debtID = Debt.ID "
		"JOIN ShareSettlement ON ShareSettlementAllocation.shareSettlementID = ShareSettlement.ID "
		"WHERE " + debtClause + " AND " + settlementClause + " "
		"), 0), 6) AS foreignDue "
	);
	sqliteUtils::bindDateRange(query, upTo, "asOf");

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

double SqliteDebtRepository::getOpenDebt(const DateRange& range) const
{
	QDebug deb = qDebug();
	deb << "-> getOpenDebt";

	const DateRange upTo = DateRange::upTo(range.untilInclusive);
	const QString debtClause = debtBookedUpToClause(upTo);
	const QString paymentClause = sqliteUtils::addedRangeClause(upTo, "Payment.dateAdded", "asOf");

	// amount of all debts booked up to the date, minus payment allocations (to those debts)
	// whose payment was added up to the date; overpayment tips are not allocations and stay out
	QSqlQuery query;
	query.prepare(
		"SELECT ROUND( "
		"COALESCE((SELECT SUM(Debt.amount) FROM Debt WHERE " + debtClause + "), 0) - "
		"COALESCE(( "
		"SELECT SUM(PaymentAllocation.amount) "
		"FROM PaymentAllocation "
		"JOIN Debt ON PaymentAllocation.debtID = Debt.ID "
		"JOIN Payment ON PaymentAllocation.paymentID = Payment.ID "
		"WHERE " + debtClause + " AND " + paymentClause + " "
		"), 0), 6) AS openDebt "
	);
	sqliteUtils::bindDateRange(query, upTo, "asOf");

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}

	deb << "--> failed: " << query.lastError().text();
	return 0.0;
}

std::vector<entry::Outstanding> SqliteDebtRepository::getPersonsOutstandingEntries(int64_t personEntryID, FilterType type) const
{
	QDebug deb = qDebug();
	deb << "-> getPersonsOutstandingEntries";

	QString sqlString;
	if (type == FilterType::omitFullyPaid) sqlString = " AND (Debt.amount - COALESCE(GroupedPayments.paid,0)) > 0";

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
		while (query.next())
			entries.emplace_back(getOutstandingEntryFromQuery(query));
	else
		deb << "--> failed: " << query.lastError().text();

	return entries;
}

std::vector<entry::Outstanding> SqliteDebtRepository::getForeignShareOutstandingEntries(FilterType type) const
{
	QDebug deb = qDebug();
	deb << "-> getForeignShareOutstandingEntries";

	QString filter;
	if (type == FilterType::omitFullyPaid)
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
		while (query.next())
			entries.emplace_back(getOutstandingEntryFromQuery(query));
	else
		deb << "--> failed: " << query.lastError().text();

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