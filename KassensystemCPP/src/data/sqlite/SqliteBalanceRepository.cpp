#include "SqliteBalanceRepository.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteBalanceRepository::addBalanceEntry(entry::Balance entry)
{
	QSqlQuery query;
	query.prepare(
		"INSERT INTO Balance"
		"(type, description, amount, dateBooked, dateAdded, comment, personID)"
		"VALUES (:balanceType, :description, :amount, :dateBooked, :dateAdded, :comment, :personID)"
		"RETURNING ID"
	);
	query.bindValue(":balanceType", static_cast<int>(entry.type));
	query.bindValue(":description", QString::fromStdString(entry.description));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":dateBooked", entry.dateBooked.toString(Qt::ISODate));
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":comment", QString::fromStdString(entry.comment));
	query.bindValue(":personID", entry.personEntryID);

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
	
}

std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> SqliteBalanceRepository::getBalanceEntry(const std::string& description) const
{
	QSqlQuery query;
	query.prepare(
		"SELECT *"
		"FROM Balance"
		"WHERE Balance.description = :description"
	);
	query.bindValue(":description", QString::fromStdString(description));

	std::optional<entry::Balance> foundEntry;
	if (query.exec())
	{
		while (query.next())
		{
			if (foundEntry.has_value()) return std::unexpected(GetEntryException::MultipleEntriesFound);

			foundEntry = entry::Balance{
				.balanceEntryID = query.value("ID").toLongLong(),
				.type = static_cast<BalanceType>(query.value("type").toInt()),
				.description = query.value("description").toString().toStdString(),
				.amount = query.value("amount").toDouble(),
				.dateBooked = query.value("dateBooked").toDate(),
				.dateAdded = query.value("dateAdded").toDate(),
				.comment = query.value("comment").toString().toStdString(),
				.personEntryID = query.value("personID").toLongLong()
			};
		}
	}
	
	if (foundEntry.has_value()) return foundEntry.value();

	return std::unexpected(GetEntryException::EntryNotFound);
}

std::vector<entry::Balance> SqliteBalanceRepository::getBalanceEntries(BalanceType type, const QDate& minDate) const
{

}