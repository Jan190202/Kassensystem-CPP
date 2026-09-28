#include "SqliteShareSettlementRepository.h"
#include "data/sqlite/sqliteutils/SqliteUtils.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteShareSettlementRepository::addShareSettlementEntry(const entry::ShareSettlement& entry)
{
	bool isBookedSpecial = entry.dateBooked.isSpecial();
	
	QSqlQuery query;
	query.prepare(
		"INSERT INTO ShareSettlement "
		"(dateBooked, dateBookedSpecial, dateAdded, amount) "
		"VALUES (:dateBooked, :dateBookedSpecial, :dateAdded, :amount) "
		"RETURNING ID "
	);

	query.bindValue(":dateBooked", entry.dateBooked.toSqlDateValue());
	query.bindValue(":dateBookedSpecial", entry.dateBooked.toSqlSpecialValue());
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

int64_t SqliteShareSettlementRepository::addShareSettlementAllocationEntry(const entry::ShareSettlementAllocation& entry)
{
	QSqlQuery query;
	query.prepare(
		"INSERT INTO ShareSettlementAllocation "
		"(debtID, shareSettlementID, amount) "
		"VALUES (:debtID, :shareSettlementID, :amount) "
		"RETURNING ID "
	);
	query.bindValue(":debtID", entry.debtEntryID);
	query.bindValue(":shareSettlementID", entry.shareSettlementEntryID);
	query.bindValue(":amount", entry.amount);

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

std::vector<entry::ShareSettlementAllocation> SqliteShareSettlementRepository::getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const
{
	QSqlQuery query;
	query.prepare(
		"SELECT ID, debtID, shareSettlementID, amount "
		"FROM ShareSettlementAllocation "
		"WHERE debtID = :debtEntryID "
	);
	query.bindValue(":debtID", debtEntryID);

	std::vector<entry::ShareSettlementAllocation> entries;
	if (query.exec())
		while (query.next())
		{
			entries.emplace_back(entry::ShareSettlementAllocation{
				.shareSettlementAllocationEntryID = query.value("ID").toLongLong(),
				.debtEntryID = query.value("debtID").toLongLong(),
				.shareSettlementEntryID = query.value("shareSettlementID").toLongLong(),
				.amount = query.value("amount").toDouble()
				});
		}

	qDebug() << "DebtsEntryAllocationEntries: ";
	for (auto& entry : entries)
		qDebug() << entry;

	return entries;
}

double SqliteShareSettlementRepository::getTotalAllocatedShareSettlements(const QDate& minDate) const
{
	QSqlQuery query;
	
	QString compClause = sqliteUtils::registerDateCompareClause(
		sqliteUtils::Op::largerOrEq, 
		RegisterDate{ minDate }, 
		"ShareSettlement.dateBooked", 
		"ShareSettlement.dateBookedSpecial", 
		":minDate");

	query.prepare(
		"SELECT SUM(ShareSettlementAllocation.amount) AS total "
		"FROM ShareSettlementAllocation "
		"JOIN ShareSettlement ON ShareSettlementAllocation.shareSettlementID = ShareSettlement.ID "
		"WHERE " + compClause + " "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	if (query.exec())
	{
		if (query.next())
		{
			qDebug() << "AllocatedSettlements: " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
	}
}