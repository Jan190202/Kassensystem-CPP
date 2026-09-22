#include "Utils.h"
#include <algorithm>

namespace Utils
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
}
