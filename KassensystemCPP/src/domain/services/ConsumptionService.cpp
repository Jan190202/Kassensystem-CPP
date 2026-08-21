#include "ConsumptionService.h"
#include "domain/model/PriceList.h"

ConsumptionService::ConsumptionService(ConsumptionRepository* consumptionRepo, DebtRepository* debtRepo, PersonRepository* personRepo)
{
	this->consumptionRepo = consumptionRepo;
	this->debtRepo = debtRepo;
	this->personRepo = personRepo;
}

void ConsumptionService::addConsumption(const ConsumptionRequest& request)
{
	Person person = personRepo->findOrCreateEntry(request.personName);
	double amount = calculateDebt(request);

	DebtEntry dEntry{ .debtEntryID = 0, .personID = person.getID(), .date = request.date, .amount = amount};
	int dEntryID = debtRepo->addEntry(dEntry);

	ConsumptionEntry cEntry{ .consumptionEntryID = 0, .debtEntryID = dEntryID, .nBeer05 = request.nBeer05 , .nBeer04 = request.nBeer04, .nSoftdrinks = request.nSoftdrinks, .nWater = request.nWater };
	int cEntryID = consumptionRepo->addEntry(cEntry);
}

std::vector<ConsumptionEntry> ConsumptionService::getEntries(int personID) const
{
	return consumptionRepo->getEntries(personID);
}

double ConsumptionService::calculateDebt(const ConsumptionRequest& request) const
{
	return
		PriceList::beer04		* request.nBeer04 +
		PriceList::beer05		* request.nBeer05 +
		PriceList::water		* request.nWater  +
		PriceList::softdrink	* request.nSoftdrinks + 
		request.otherExpense;
}

std::vector<std::string> ConsumptionService::getPersonNames() const
{
	return personRepo->getNames();
}