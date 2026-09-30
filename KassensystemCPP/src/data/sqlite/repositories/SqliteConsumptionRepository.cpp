#include "SqliteConsumptionRepository.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteConsumptionRepository::addConsumptionEntry(const entry::Consumption& entry)
{
	QDebug deb = qDebug();
	deb << "-> addConsumptionEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO Consumption"
		"(debtID, nBeer05, nBeer04, nSoftdrinks, nWater, otherExpense)"
		"VALUES (:debtID, :nBeer05, :nBeer04, :nSoftdrinks, :nWater, :otherExpense)"
		"RETURNING ID"
	);
	query.bindValue(":debtID", entry.debtEntryID);
	query.bindValue(":nBeer05", entry.nBeer05);
	query.bindValue(":nBeer04", entry.nBeer04);
	query.bindValue(":nSoftdrinks", entry.nSoftdrinks);
	query.bindValue(":nWater", entry.nWater);
	query.bindValue(":otherExpense", entry.otherExpense);

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}

	deb << "--> failed";
}

std::vector<entry::Consumption> SqliteConsumptionRepository::getConsumptionEntries(int64_t personEntryID) const
{
	QDebug deb = qDebug();
	deb << "-> getConsumptionEntries";

	QSqlQuery query;
	query.prepare(
		"SELECT Consumption.* "
		"FROM Consumption "
		"INNER JOIN Debt ON Consumption.debtID = Debt.ID "
		"WHERE Debt.personID = :personEntryID"
	);
	query.bindValue(":personEntryID", personEntryID);

	std::vector<entry::Consumption> entries;
	if (query.exec())
	{
		while (query.next())
		{
			entries.emplace_back(
				entry::Consumption{
					.consumptionEntryID = query.value("ID").toLongLong(),
					.debtEntryID = query.value("debtID").toLongLong(),
					.nBeer05 = query.value("nBeer05").toInt(),
					.nBeer04 = query.value("nBeer04").toInt(),
					.nSoftdrinks = query.value("nSoftdrinks").toInt(),
					.nWater = query.value("nWater").toInt(),
					.otherExpense = query.value("otherExpense").toDouble()
				});
		}
	}
	else
		deb << "--> failed";

	return entries;
}