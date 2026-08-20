#pragma once

#include "Person.h"
#include "ConsumptionEntry.h"
#include <vector>

struct ConsumptionSummary
{
	const Person& person;
	double total, paid, due, credit;
	std::vector<ConsumptionEntry> entries;
};