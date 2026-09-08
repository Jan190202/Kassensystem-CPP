#pragma once

#include <vector>

namespace idgen
{
	int64_t getID(const std::vector<int64_t>& usedIDs);
}