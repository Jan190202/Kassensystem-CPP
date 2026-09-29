#include "SqliteBalanceRepository.h"
#include "data/sqlite/sqliteutils/SqliteUtils.h"
#include <QSqlDatabase>
#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>

int64_t SqliteBalanceRepository::addBalanceEntry(const entry::Balance& entry)
{
	qDebug() << "-> addBalanceEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO Balance"
		"(type, description, amount, dateBooked, dateBookedSpecial, dateAdded, comment, personID)"
		"VALUES (:balanceType, :description, :amount, :dateBooked, :dateBookedSpecial, :dateAdded, :comment, :personID)"
		"RETURNING ID"
	);
	query.bindValue(":balanceType", static_cast<int>(entry.type));
	query.bindValue(":description", QString::fromStdString(entry.description));
	query.bindValue(":amount", entry.amount);
	query.bindValue(":dateBooked", entry.dateBooked.toSqlDateValue());
	query.bindValue(":dateBookedSpecial", entry.dateBooked.toSqlSpecialValue());
	query.bindValue(":dateAdded", entry.dateAdded.toString(Qt::ISODate));
	query.bindValue(":comment", QString::fromStdString(entry.comment));
	if (entry.personEntryID.has_value())
		query.bindValue(":personID", entry.personEntryID.value());
	else
		query.bindValue(":personID", QVariant(QMetaType::fromType<qlonglong>()));

	if (query.exec())
		if (query.next())
		{
			qDebug() << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}
}

std::expected<entry::Balance, GetEntryException> SqliteBalanceRepository::getBalanceEntry(const std::string& description) const
{
	qDebug() << "-> getBalanceEntry";

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
			if (foundEntry.has_value()) return std::unexpected(GetEntryException::multipleEntriesFound);

			foundEntry = getEntryFromQuery(query);
		}
	}
	
	if (foundEntry.has_value()) return foundEntry.value();

	return std::unexpected(GetEntryException::entryNotFound);
}

std::vector<entry::Balance> SqliteBalanceRepository::getBalanceEntries(BalanceType type, const QDate& minDate) const
{
	qDebug() << "-> getBalanceEntries";

	bool isTypeSpecific = hasFlag(type, BalanceType::earning) ^ hasFlag(type, BalanceType::spending); // either Earning or Spending, but not both
	
	QString compClause = sqliteUtils::registerDateCompareClause(
		sqliteUtils::Op::largerOrEq,
		RegisterDate{ minDate },
		"Balance.dateBooked",
		"Balance.dateBookedSpecial",
		":minDate");

	QString sqlStatement =
		"SELECT * "
		"FROM Balance "
		"WHERE " + compClause + " ";

	if (isTypeSpecific)
	{
		sqlStatement += "AND Balance.type = :type ";
	}

	QSqlQuery query;
	query.prepare(sqlStatement);
	query.bindValue(":minDate", minDate.toString(Qt::ISODate));
	
	if (isTypeSpecific)
	{
		query.bindValue(":type", 
			static_cast<uint8_t>(type & (BalanceType::earning | BalanceType::spending))); // only flag bits for Earning or Spending are left
	}
	
	std::vector<entry::Balance> entries;
	if (query.exec())
		while (query.next())
			entries.emplace_back(getEntryFromQuery(query));

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
		.dateBooked =
			!query.value("dateBooked").isNull() ?
				RegisterDate{query.value("dateBooked").toDate()} :
				RegisterDate{static_cast<RegisterDate::Special>(query.value("dateBookedSpecial").toInt())},
		.dateAdded = query.value("dateAdded").toDate(),
		.comment = query.value("comment").toString().toStdString(),
		.personEntryID = query.value("personID").toLongLong()
	};
}