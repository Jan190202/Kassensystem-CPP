#pragma once
#include "domain/model/Entities.h"
#include <QString>

class QSqlQuery;

namespace sqliteUtils
{
	enum class Op
	{
		smaller, smallerOrEq, larger, largerOrEq, equal
	};

	QString registerDateCompareClause(Op op, const RegisterDate& comparisonDate,
		const QString& dateColumnName,
		const QString& specialColumnName,
		const QString& bindName);

	QString bookedRangeClause(const DateRange& range,
		const QString& dateColumn,
		const QString& specialColumn,
		const QString& addedColumn,
		const QString& bindPrefix);

	QString addedRangeClause(const DateRange& range, const QString& addedColumn, const QString& bindPrefix);

	void bindDateRange(QSqlQuery& query, const DateRange& range, const QString& bindPrefix);
}