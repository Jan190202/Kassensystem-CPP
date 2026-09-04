#pragma once

#include <vector>

namespace idgen
{
	int64_t getID(std::vector<int64_t> usedIDs);
	int64_t generate();
	bool isUnique(std::vector<int64_t> usedIDs, int64_t newID);
}