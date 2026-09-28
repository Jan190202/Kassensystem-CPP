#pragma once
#include "domain/model/Entities.h"
#include <QString>

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
}