#pragma once

#include "Entities.h"

namespace financialStateBefore
{
	registerFinancials::State read()
	{
		auto stateBefore = registerFinancials::State{ .date = QDate(2026, 1, 1), .cash = 1000, .savings = 1500, .ownCash = 900, .foreignCash = 100 };
		return stateBefore;
	}
}