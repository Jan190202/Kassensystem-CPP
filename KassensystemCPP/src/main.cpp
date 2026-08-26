#include "gui/CashRegisterSystemUI.h"
#include "gui/GuiTypes.h"

#include "domain/model/Entities.h"
#include "domain/model/Requests.h"
#include "domain/model/DomainTypes.h"

#include "domain/services/ConsumptionService.h"
#include "domain/services/PaymentService.h"
#include "domain/services/BalanceService.h"

#include "test/BalanceRepoInMem.h"
#include "test/ConsumptionRepoInMem.h"
#include "test/CreditRepoInMem.h"
#include "test/DebtRepoInMem.h"
#include "test/PaymentRepoInMem.h"
#include "test/PersonRepoInMem.h"

#include <Windows.h>
#include <QApplication>
#include <QDebug>
#include <string>
#include <vector>
#include <iostream>

#include "ocr/EntryRecognizer.h"

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);
	QApplication app(argc, argv);
	//////////////////////////////////////////////////////////////////////////////////////////////

	// OCR Testing
	std::vector<ConsumptionInputs> inputVector = EntryRecognizer::instance().readDocument("TestPath");
	EntryRecognizer::instance().shutdownInterpreter();

	for (auto& inputs : inputVector)
	{
		std::cout << inputs << std::endl;
	}


	// GUI Testing
	BalanceRepository* baRep		= new BalanceRepoInMem();
	ConsumptionRepository* coRep	= new ConsumptionRepoInMem();
	CreditRepository* crRep			= new CreditRepoInMem();
	DebtRepository* deRep			= new DebtRepoInMem();
	PaymentRepository* paRep		= new PaymentRepoInMem();
	PersonRepository* peRep			= new PersonRepoInMem();

	BalanceService			baSer(baRep, crRep, peRep);
	ConsumptionService		coSer(coRep, deRep, peRep);
	PaymentService			paSer(paRep, crRep, deRep, baRep, peRep);

	auto services = ServiceBundle{ .paymentService = paSer, .consumptionService = coSer, .balanceService = baSer };
	CashRegisterSystemUI sysUI(services);
	sysUI.show();

	///////////////////////////////////////////////////////////////////////////////////////////////
	return app.exec();
}