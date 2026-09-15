#pragma once
#include "domain/repointerface/ConsumptionRepository.h"
#include <QSqlQuery>

class SqliteConsumptionRepository : public ConsumptionRepository
{
public:
	SqliteConsumptionRepository() = default;
	virtual ~SqliteConsumptionRepository() = default;
	virtual int64_t addConsumptionEntry(const entry::Consumption& entry) override;

	virtual std::vector<entry::Consumption> getConsumptionEntries(int64_t personEntryID) const override;
};