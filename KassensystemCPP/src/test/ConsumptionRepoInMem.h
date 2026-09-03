#pragma once

#include "domain/repoInterface/ConsumptionRepository.h"
#include "domain/model/Entities.h"
#include <vector>

class ConsumptionRepoInMem : public ConsumptionRepository
{
public:
	ConsumptionRepoInMem() = default;
	virtual ~ConsumptionRepoInMem() = default;

	virtual int64_t addEntry(const entry::Consumption&) override;
	virtual std::vector<entry::Consumption> getEntries(int64_t personID) const override;
private:
	std::vector<entry::Consumption> data;
};