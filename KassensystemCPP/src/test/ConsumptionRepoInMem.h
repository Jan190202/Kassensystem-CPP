#pragma once

#include "domain/repoInterface/ConsumptionRepository.h"
#include "domain/model/Entities.h"
#include <vector>

class ConsumptionRepoInMem : public ConsumptionRepository
{
public:
	ConsumptionRepoInMem() = default;
	virtual ~ConsumptionRepoInMem() = default;

	virtual int64_t addEntry(const ConsumptionEntry&) override;
	virtual std::vector<ConsumptionEntry> getEntries(int64_t personID) const override;
private:
	std::vector<ConsumptionEntry> data;
};