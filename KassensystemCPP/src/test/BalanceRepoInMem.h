#pragma once
#include "domain/repointerface/BalanceRepository.h"
#include "domain/model/Entities.h"

class BalanceRepoInMem : public BalanceRepository
{
public:
	BalanceRepoInMem() = default;
	virtual ~BalanceRepoInMem() = default;

	virtual int64_t addBalanceEntry(entry::Balance) override;
	virtual std::expected<std::reference_wrapper<const entry::Balance>, GetEntryException> getBalanceEntry(const std::string& description) const override;
	virtual std::vector<entry::Balance> getBalanceEntries(BalanceType type) const override;
private:
	std::vector<entry::Balance> entries{};
};