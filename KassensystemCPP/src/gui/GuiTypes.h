#pragma once

#include <QPushButton>
#include <string>
#include <variant>

enum class BtnIndex
{
	AddEarning, AddSpending
};

struct LowerButtonBundle
{
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;
};

struct ConsumptionInputs
{
	std::variant<int64_t, std::string> personInput;
	int nBeer05, nBeer04, nSoftdrinks, nWater;
	double otherExpense;
};