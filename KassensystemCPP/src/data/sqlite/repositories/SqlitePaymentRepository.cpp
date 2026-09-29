#include "SqlitePaymentRepository.h"
#include "data/sqlite/sqliteutils/SqliteUtils.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqlitePaymentRepository::addPaymentEntry(const entry::Payment& entry)
{
	qDebug() << "-> addPaymentEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO Payment "
		"(personID, dateAdded, amount, comment, overpaymentType) "
		"VALUES (:personID, :dateAdded, :amount, :comment, :overpaymentType) "
		"RETURNING ID "
	);
	query.bindValue(":personID", entry.personEntryID);
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":comment", QString::fromStdString(entry.comment));
	query.bindValue(":overpaymentType", static_cast<int>(entry.overpaymentType));
	
	if (query.exec())
		if (query.next())
		{
			qDebug() << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}
}

int64_t SqlitePaymentRepository::addPaymentAllocationEntry(const entry::PaymentAllocation& entry)
{
	qDebug() << "-> addPaymentAllocationEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO PaymentAllocation "
		"(debtID, paymentID, amount) "
		"VALUES (:debtID, :paymentID, :amount) "
		"RETURNING ID "
	);
	query.bindValue(":debtID", entry.debtEntryID);
	query.bindValue(":paymentID", entry.paymentEntryID);
	query.bindValue(":amount", entry.amount);

	if (query.exec())
		if (query.next())
		{
			qDebug() << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}
}

std::vector<entry::PaymentAllocation> SqlitePaymentRepository::getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const
{
	qDebug() << "-> getDebtsEntrysPaymentAllocationEntries";

	QSqlQuery query;
	query.prepare(
		"SELECT ID, debtID, paymentID, amount "
		"FROM PaymentAllocation "
		"WHERE debtID = :debtEntryID "
	);
	query.bindValue(":debtID", debtEntryID);

	std::vector<entry::PaymentAllocation> entries;
	if (query.exec())
		while (query.next())
		{
			entries.emplace_back(entry::PaymentAllocation{
				.paymentAllocationEntryID = query.value("ID").toLongLong(),
				.debtEntryID = query.value("debtID").toLongLong(),
				.paymentEntryID = query.value("paymentID").toLongLong(),
				.amount = query.value("amount").toDouble()
				});
		}

	return entries;
}

double SqlitePaymentRepository::getTotalAllocatedPayments(const QDate& minDate) const
{
	qDebug() << "-> getTotalAllocatedPayments";

	QSqlQuery query;
	query.prepare(
		"SELECT COALESCE(SUM(PaymentAllocation.amount),0) AS total "
		"FROM PaymentAllocation "
		"LEFT OUTER JOIN Payment ON PaymentAllocation.paymentID = Payment.ID "
		"WHERE Payment.dateAdded >= :minDate "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	if (query.exec())
		if (query.next())
		{
			qDebug() << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
}