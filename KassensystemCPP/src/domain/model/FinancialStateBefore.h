#pragma once

#include "Entities.h"

namespace financialStateBefore
{
	RegisterFinancialState read()
	{
		auto stateBefore = RegisterFinancialState{ .date = QDate(2026, 1, 1), .cash = 1000, .savings = 1500, .ownCash = 900, .foreignCash = 100 };
		return stateBefore;
	}
}