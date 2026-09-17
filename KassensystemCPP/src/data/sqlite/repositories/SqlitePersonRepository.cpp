#include "SqlitePersonRepository.h"
#include <QSqlDatabase>

#include <QFile>
#include <QSqlError>
#include <QDebug>
#include <vector>
#include <optional>

int64_t SqlitePersonRepository::addPersonEntry(const entry::Person& entry)
{
	QSqlQuery query;
	query.prepare(
		"INSERT INTO Person "
		"(firstName, lastName, nickName, info) "
		"VALUES (:firstName, :lastName, :nickName, :info) "
		"RETURNING ID"
	);
	query.bindValue(":firstName", QString::fromStdString(entry.firstName));
	query.bindValue(":lastName", QString::fromStdString(entry.lastName));
	query.bindValue(":nickName", QString::fromStdString(entry.nickName));
	query.bindValue(":info", QString::fromStdString(entry.info));

	if (query.exec())
		if (query.next())
			return query.value(0).toLongLong();
}

std::expected<entry::Person, GetEntryException> SqlitePersonRepository::findPersonEntry(int64_t personEntryID) const
{
	QSqlQuery query;
	query.prepare(
		"SELECT ID, firstName, lastName, nickName, info "
		"FROM Person "
		"WHERE ID = :personEntryID"
	);
	query.bindValue(":personEntryID", personEntryID);

	std::optional<entry::Person> foundPerson;
	if (query.exec())
	{
		if (query.next())
		{
			if (foundPerson.has_value()) return std::unexpected(GetEntryException::MultipleEntriesFound);

			foundPerson = getEntryFromQuery(query);
		}
	}

	if (!foundPerson.has_value()) return std::unexpected(GetEntryException::EntryNotFound);

	return foundPerson.value();
}

std::vector<entry::Person> SqlitePersonRepository::getAllPersonEntries() const
{
	QSqlQuery query;
	query.prepare(
		"SELECT ID, firstName, lastName, nickName, info "
		"FROM Person "
	);

	std::vector<entry::Person> entries;
	if (query.exec())
	{
		while (query.next())
		{
			entries.emplace_back(getEntryFromQuery(query));
		}
	}

	qDebug() << "all person entries: ";
	for (auto& entry : entries)
		qDebug() << entry;

	return entries;

}

entry::Person SqlitePersonRepository::getEntryFromQuery(const QSqlQuery& query) const
{
	return entry::Person{
				.personEntryID = query.value("ID").toLongLong(),
				.firstName = query.value("firstName").toString().toStdString(),
				.lastName = query.value("lastName").toString().toStdString(),
				.nickName = query.value("nickName").toString().toStdString(),
				.info = query.value("info").toString().toStdString()
				};
}