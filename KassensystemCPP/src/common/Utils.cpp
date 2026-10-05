#include "Utils.h"
#include <algorithm>
#include <format>

namespace utils
{
	std::string eurSymbol()
	{
		return std::string{ "\u20AC" };
	}

	void toUpper(std::string& s)
	{
		std::ranges::transform(s, s.begin(), [](unsigned char c) { return std::toupper(c); });
	}

	bool isUpper(const std::string& s)
	{
		bool isUpper = true;
		for (const auto& c : s) 
			if (!std::isupper(c)) 
				isUpper = false;
		return isUpper;
	}

	std::string toCurrencyFormat(double amount, int decimals)
	{
		std::string formatString = "{" + std::string(":.") + std::to_string(decimals) + "f" + "} {}"; // e.g. "{:.2f} {}"
		std::string symbol = eurSymbol();
		return std::vformat(formatString, std::make_format_args(amount, symbol));
	}
}
