#pragma once
#include "domain/repointerface/ConsumptionRepository.h"
#include "domain/model/Entities.h"
#include <vector>

class ConsumptionRepoInMem : public ConsumptionRepository
{
public:
	ConsumptionRepoInMem() = default;
	virtual ~ConsumptionRepoInMem() = default;

	virtual int64_t addConsumptionEntry(entry::Consumption) override;
	virtual std::vector<entry::Consumption> getConsumptionEntries(int64_t personEntryID) const override;
private:
	std::vector<entry::Consumption> entries{};
};