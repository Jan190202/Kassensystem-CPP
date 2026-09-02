#pragma once

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/repoInterface/ConsumptionRepository.h"
#include "domain/repoInterface/DebtRepository.h"
#include "domain/repoInterface/PersonRepository.h"
#include <vector>
#include <expected>

struct PersonStringSpecifiers
{
	std::string firstName, lastName, nickName, info;
};

enum NameValidationError
{
	FirstOrLastNameMissing,
	UnbalancedParentheses,
	InvalidNicknameFormat,
	TooManyComponents
};

class ConsumptionService
{
public:
	ConsumptionService(ConsumptionRepository* consumptionRepo, DebtRepository* debtRepo, PersonRepository* personRepo);
	void addConsumption(const ConsumptionRequest& request);
	
	double calculateDebt(const ConsumptionRequest&) const;
	std::vector<ConsumptionEntry> getEntries(int personID) const;
private:

	std::expected< PersonStringSpecifiers, NameValidationError > isValidNameFormat(const std::string& nameRequest);

	ConsumptionRepository* consumptionRepo;
	DebtRepository* debtRepo;
	PersonRepository* personRepo;
};






