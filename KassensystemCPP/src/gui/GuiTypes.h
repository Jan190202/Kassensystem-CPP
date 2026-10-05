#pragma once
#include "domain/model/Entities.h"
#include <QPushButton>
#include <string>
#include <variant>

enum class BtnIndex
{
	addEarning, addSpending
};

struct LowerButtonBundle
{
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;
};

struct ConsumptionInputs
{
	std::variant<entry::Person, std::string> personInput;
	int nBeer05, nBeer04, nSoftdrinks, nWater;
	double otherExpense;
};