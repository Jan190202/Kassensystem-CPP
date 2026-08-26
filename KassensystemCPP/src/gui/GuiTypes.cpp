#include "GuiTypes.h"

std::ostream& operator<<(std::ostream& out, const ConsumptionInputs& input)
{
	out << "Name: " << input.personName;
	out << " nBeer05: " << input.nBeer05;
	out << " nBeer04: " << input.nBeer04;
	out << " nSoftdrinks: " << input.nSoftdrinks;
	out << " nWater: " << input.nWater;
	out << " otherExpense: " << input.otherExpense;

	return out;
}