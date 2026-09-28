#pragma once
#include "data/sqlite/sqliteutils/SqliteUtils.h"

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
}