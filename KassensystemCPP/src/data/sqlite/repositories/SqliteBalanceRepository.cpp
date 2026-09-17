#include "SqliteBalanceRepository.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteBalanceRepository::addBalanceEntry(const entry::Balance& entry)
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

	if (entry.personEntryID.has_value())
		query.bindValue(":personID", entry.personEntryID.value());
	else
		query.bindValue(":personID", QVariant(QMetaType::fromType<qlonglong>()));

	qDebug() << entry;

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

std::expected<entry::Balance, GetEntryException> SqliteBalanceRepository::getBalanceEntry(const std::string& description) const
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

			foundEntry = getEntryFromQuery(query);
		}
	}
	
	if (foundEntry.has_value()) return foundEntry.value();

	return std::unexpected(GetEntryException::EntryNotFound);
}

std::vector<entry::Balance> SqliteBalanceRepository::getBalanceEntries(BalanceType type, const QDate& minDate) const
{
	bool isTypeSpecific = hasFlag(type, BalanceType::Earning) ^ hasFlag(type, BalanceType::Spending); // either Earning or Spending, but not both
	
	QString sqlStatement =
		"SELECT * "
		"FROM Balance "
		"WHERE Balance.dateBooked >= :minDate";

	if (isTypeSpecific)
	{
		sqlStatement += " AND Balance.type = :type";
	}

	QSqlQuery query;
	query.prepare(sqlStatement);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));
	
	if (isTypeSpecific)
	{
		query.bindValue(":type", 
			static_cast<uint8_t>(type & (BalanceType::Earning | BalanceType::Spending))); // only flag bits for Earning or Spending are left
	}
	
	std::vector<entry::Balance> entries;

	if (query.exec())
	{
		while (query.next())
		{
			entries.emplace_back(getEntryFromQuery(query));
		}
	}

	return entries;
}

entry::Balance SqliteBalanceRepository::getEntryFromQuery(const QSqlQuery& query) const
{
	return entry::Balance
	{
		.balanceEntryID = query.value("ID").toLongLong(),
		.type = static_cast<BalanceType>(query.value("type").toUInt()),
		.description = query.value("description").toString().toStdString(),
		.amount = query.value("amount").toDouble(),
		.dateBooked = query.value("dateBooked").toDate(),
		.dateAdded = query.value("dateAdded").toDate(),
		.comment = query.value("comment").toString().toStdString(),
		.personEntryID = query.value("personID").toLongLong()
	};
}