#include "SqliteFinancialReviewRepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

int64_t SqliteFinancialReviewRepository::addFinancialReviewEntry(const entry::FinancialReview& entry)
{
	QDebug deb = qDebug();
	deb << "-> addFinancialReviewEntry";

	QSqlQuery query;
	query.prepare(
		"INSERT INTO FinancialReview "
		"(dateTime, countedCash, expectedCash, comment) "
		"VALUES (:dateTime, :countedCash, :expectedCash, :comment) "
		"RETURNING ID "
	);
	query.bindValue(":dateTime", entry.dateTime.toString(Qt::ISODate)); // local time, e.g. 2026-10-10T23:59:59; sorts correctly as text
	query.bindValue(":countedCash", entry.countedCash);
	query.bindValue(":expectedCash", entry.expectedCash);
	query.bindValue(":comment", QString::fromStdString(entry.comment));

	if (query.exec())
		if (query.next())
		{
			deb << "--> returns " << query.value(0).toLongLong();
			return query.value(0).toLongLong();
		}

	deb << "--> failed: " << query.lastError().text();
	return -1;
}

std::vector<entry::FinancialReview> SqliteFinancialReviewRepository::getAllFinancialReviewEntries() const
{
	QDebug deb = qDebug();
	deb << "-> getAllFinancialReviewEntries";

	QSqlQuery query;
	query.prepare(
		"SELECT ID, dateTime, countedCash, expectedCash, comment "
		"FROM FinancialReview "
		"ORDER BY dateTime "
	);

	std::vector<entry::FinancialReview> entries;
	if (query.exec())
		while (query.next())
		{
			entries.emplace_back(entry::FinancialReview{
				.financialReviewEntryID = query.value("ID").toLongLong(),
				.dateTime = QDateTime::fromString(query.value("dateTime").toString(), Qt::ISODate),
				.countedCash = query.value("countedCash").toDouble(),
				.expectedCash = query.value("expectedCash").toDouble(),
				.comment = query.value("comment").toString().toStdString()
				});
		}
	else
		deb << "--> failed: " << query.lastError().text();

	return entries;
}