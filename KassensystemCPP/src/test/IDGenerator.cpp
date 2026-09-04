#include "IDGenerator.h"

#include <random>

namespace idgen
{
	int64_t getID(std::vector<int64_t> usedIDs)
	{
		int64_t newID;
		do
		{
			newID = idgen::generate();
		} while (!idgen::isUnique(usedIDs, newID));

		return newID;
	}

	int64_t generate()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int64_t> distrib(1, 1000);
		return distrib(gen);
	}

	bool isUnique(std::vector<int64_t> usedIDs, int64_t newID)
	{
		for (int64_t usedID : usedIDs)
		{
			if (usedID == newID) return false;
		}
		return true;
	}
}