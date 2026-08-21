#pragma once

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/repoInterface/ConsumptionRepository.h"
#include "domain/repoInterface/DebtRepository.h"
#include "domain/repoInterface/PersonRepository.h"
#include <vector>

class ConsumptionService
{
public:
	ConsumptionService(ConsumptionRepository* consumptionRepo, DebtRepository* debtRepo, PersonRepository* personRepo);
	void addConsumption(const ConsumptionRequest& request);
	
	double calculateDebt(const ConsumptionRequest&) const;
	std::vector<ConsumptionEntry> getEntries(int personID) const;
	std::vector<std::string> getPersonNames() const;
private:
	ConsumptionRepository* consumptionRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
};






