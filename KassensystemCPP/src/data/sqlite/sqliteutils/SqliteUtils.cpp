#include "data/sqlite/sqliteutils/SqliteUtils.h"
#include <QSqlQuery>
#include <QStringList>
#include <utility>

namespace sqliteUtils
{
	namespace
	{
		bool holds(const RegisterDate& lhs, Op op, const RegisterDate& rhs)
		{
			switch (op)
			{
			case Op::smaller:     return lhs < rhs;
			case Op::smallerOrEq: return lhs <= rhs;
			case Op::larger:      return lhs > rhs;
			case Op::largerOrEq:  return lhs >= rhs;
			case Op::equal:       return lhs <= rhs && lhs >= rhs;
			}
			std::unreachable();
		}

		const char* sqlOperator(Op op)
		{
			switch (op)
			{
			case Op::smaller:     return "<";
			case Op::smallerOrEq: return "<=";
			case Op::larger:      return ">";
			case Op::largerOrEq:  return ">=";
			case Op::equal:       return "=";
			}
			std::unreachable();
		}

		// "col > :xAfter AND col <= :xUntil" with only the bounds that are set; ISO date strings compare correctly as text
		QString plainRange(const DateRange& range, const QString& column, const QString& prefix)
		{
			QStringList conditions;
			if (range.afterExclusive.has_value())
				conditions << QString("%1 > :%2After").arg(column, prefix);
			if (range.untilInclusive.has_value())
				conditions << QString("%1 <= :%2Until").arg(column, prefix);

			return conditions.isEmpty() ? QString("1 = 1") : conditions.join(" AND ");
		}
	}

	QString registerDateCompareClause(Op op, const RegisterDate& comparisonDate,
		const QString& dateColumnName,
		const QString& specialColumnName,
		const QString& bindName)
	{
		QStringList parts;

		// database: regular
		if (!comparisonDate.isSpecial())
		{
			// regular (database) vs. regular (comparison) -> decide by date string comparison
			parts << QString("%1 %2 %3").arg(dateColumnName, sqlOperator(op), bindName);
		}
		else
		{
			// regular (database) vs. special (comparison) -> decide by rank
			const RegisterDate anyRegular{ QDate(2000, 1, 1) };
			if (holds(anyRegular, op, comparisonDate))
				parts << QString("%1 IS NULL").arg(specialColumnName);
		}

		// database: special
		QStringList matchingSpecials;
		for (RegisterDate::Special s : RegisterDate::allSpecials)
		{
			if (holds(RegisterDate{ s }, op, comparisonDate))
				matchingSpecials << QString::number(static_cast<int>(s));
		}
		if (!matchingSpecials.isEmpty())
			parts << QString("%1 IN (%2)").arg(specialColumnName, matchingSpecials.join(", "));

		if (parts.isEmpty())
			return "1 = 0";   // nothing can match; basically "false"

		return "(" + parts.join(" OR ") + ")";
	}

	QString bookedRangeClause(const DateRange& range,
		const QString& dateColumn,
		const QString& specialColumn,
		const QString& addedColumn,
		const QString& bindPrefix)
	{
		if (!range.afterExclusive.has_value() && !range.untilInclusive.has_value())
			return "1 = 1"; // open on both sides: every entry, special or not

		QStringList parts;

		// regular booking date: compared directly
		parts << QString("(%1 IS NULL AND %2)")
			.arg(specialColumn, plainRange(range, dateColumn, bindPrefix));

		// `previous` lies before every review: only part of ranges that start at the beginning
		if (!range.afterExclusive.has_value())
			parts << QString("%1 = %2")
			.arg(specialColumn, QString::number(static_cast<int>(RegisterDate::Special::previous)));

		// `unknown` and `subsequent` fall back to the day the entry was added
		parts << QString("(%1 IN (%2, %3) AND %4)")
			.arg(specialColumn,
				QString::number(static_cast<int>(RegisterDate::Special::unknown)),
				QString::number(static_cast<int>(RegisterDate::Special::subsequent)),
				plainRange(range, addedColumn, bindPrefix));

		return "(" + parts.join(" OR ") + ")";
	}

	QString addedRangeClause(const DateRange& range, const QString& addedColumn, const QString& bindPrefix)
	{
		return "(" + plainRange(range, addedColumn, bindPrefix) + ")";
	}

	void bindDateRange(QSqlQuery& query, const DateRange& range, const QString& bindPrefix)
	{
		if (range.afterExclusive.has_value())
			query.bindValue(":" + bindPrefix + "After", range.afterExclusive->toString(Qt::ISODate));
		if (range.untilInclusive.has_value())
			query.bindValue(":" + bindPrefix + "Until", range.untilInclusive->toString(Qt::ISODate));
	}
}