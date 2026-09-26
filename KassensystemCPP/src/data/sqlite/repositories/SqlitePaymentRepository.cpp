#include "SqlitePaymentRepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqlitePaymentRepository::addPaymentEntry(const entry::Payment& entry)
{
	QSqlQuery query;
	query.prepare(
		"INSERT INTO Payment "
		"(personID, date, amount, overpaymentType) "
		"VALUES (:personID, :date, :amount, :overpaymentType) "
		"RETURNING ID "
	);
	query.bindValue(":personID", entry.personEntryID);
	query.bindValue(":date", entry.dateBooked.date().toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":overpaymentType", static_cast<int>(entry.overpaymentType));
	
	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

int64_t SqlitePaymentRepository::addPaymentAllocationEntry(const entry::PaymentAllocation& entry)
{
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
			return query.value(0).toLongLong();
}

std::vector<entry::PaymentAllocation> SqlitePaymentRepository::getDebtsEntrysPaymentAllocationEntries(int64_t debtEntryID) const
{
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

	qDebug() << "DebtsEntryAllocationEntries: ";
	for (auto& entry: entries)
		qDebug() << entry;

	return entries;
}

double SqlitePaymentRepository::getTotalAllocatedPayments(const QDate& minDate) const
{
	QSqlQuery query;
	query.prepare(
		"SELECT SUM(PaymentAllocation.amount) AS total "
		"FROM PaymentAllocation "
		"LEFT OUTER JOIN Payment ON PaymentAllocation.paymentID = Payment.ID "
		"WHERE Payment.date >= :minDate "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	if (query.exec())
		if (query.next())
			return query.value(0).toDouble();
}