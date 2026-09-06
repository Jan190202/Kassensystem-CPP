#pragma once

#include "domain/repoInterface/BalanceRepository.h"
#include "domain/model/Entities.h"

class BalanceRepoInMem : public BalanceRepository
{
public:
	BalanceRepoInMem() = default;
	virtual ~BalanceRepoInMem() = default;

	virtual int64_t addEntry(entry::Balance) override;
	virtual std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> getEntry(std::string description) const override;
	virtual double getTotalEarnings() const override;
	virtual double getTotalSpendings() const override;
	virtual std::vector<entry::Balance> getEntries(BalanceType) const override;
private:
	std::vector<entry::Balance> entries{};
};