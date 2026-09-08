#pragma once

#include "domain/model/Entities.h"

#include <vector>
#include <string>

class SettlementRepository
{
public:
	SettlementRepository() = default;
	virtual ~SettlementRepository() = default;

	virtual int64_t addSettlementEntry(entry::Settlement) = 0;
	virtual int64_t addSettlementAllocationEntry(entry::SettlementAllocation) = 0;
};