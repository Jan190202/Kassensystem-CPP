#pragma once

#include <string>

namespace utils
{
	std::string eurSymbol();
	std::string toCurrencyFormat(double amount, int decimals = 2);
	void toUpper(std::string& s);
	bool isUpper(const std::string& s);
}