#pragma once

#include "domain/repoInterface/ConsumptionRepository.h"
#include "entities/ConsumptionEntry.h"
#include <vector>

class ConsumptionRepoInMem : public ConsumptionRepository
{
public:
	ConsumptionRepoInMem();
	virtual ~ConsumptionRepoInMem() override;

	virtual void addEntry(const ConsumptionEntry&) override;
	virtual std::vector<ConsumptionEntry> getEntries(int personID) const override;
private:
	std::vector<ConsumptionEntry> data;
};