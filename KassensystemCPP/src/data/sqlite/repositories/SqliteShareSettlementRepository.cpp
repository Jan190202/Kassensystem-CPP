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
	QDebug deb = qDebug();
	deb << "-> addShareSettlementEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO ShareSettlement "
		"(dateAdded, amount, comment) "
		"VALUES (:dateAdded, :amount, :comment) "
		"RETURNING ID "
	);
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":comment", QString::fromStdString(entry.comment));

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}
	
	deb << "--> failed";
}

int64_t SqliteShareSettlementRepository::addShareSettlementAllocationEntry(const entry::ShareSettlementAllocation& entry)
{
	QDebug deb = qDebug();
	deb << "-> addShareSettlementAllocationEntry";

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
		{
			deb << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}

	deb << "--> failed";
}

std::vector<entry::ShareSettlementAllocation> SqliteShareSettlementRepository::getDebtEntrysShareSettlementAllocationEntries(int64_t debtEntryID) const
{
	QDebug deb = qDebug();
	deb << "-> getDebtEntrysShareSettlementAllocationEntries";

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
	else
		deb << "--> failed";

	return entries;
}

double SqliteShareSettlementRepository::getTotalAllocatedShareSettlements(const QDate& minDate) const
{
	QDebug deb = qDebug();
	deb << "-> getTotalAllocatedShareSettlements";

	QSqlQuery query;
		query.prepare(
		"SELECT COALESCE(SUM(ShareSettlementAllocation.amount),0) AS total "
		"FROM ShareSettlementAllocation "
		"JOIN ShareSettlement ON ShareSettlementAllocation.shareSettlementID = ShareSettlement.ID "
		"WHERE ShareSettlement.dateAdded >= :minDate "
	);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));

	if (query.exec())
	{
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toDouble();
			return query.value(0).toDouble();
		}
	}

	deb << "--> failed";
}