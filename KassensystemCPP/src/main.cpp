#include "gui/Kassensystem.h"
#include <QApplication>

#include "entities/ConsumptionEntry.h"
#include "entities/ConsumptionSummary.h"
#include "entities/Person.h"
#include "entities/BalanceEntry.h"
#include "domain/ConsumptionService.h"
#include "test/ConsumptionRepoInMem.h"
#include "test/BalanceRepoInMem.h"
#include <string>
#include <QDebug>

#include <iostream>

int main(int argc, char* argv[])
{
	// Domain Testing
	Person p("Dieter", 1000);
	ConsumptionEntry e = { .personID = p.getID(), .nBeer05 = 0, .nBeer04 = 0, .nSoftdrinks = 0, .nWater = 0, .otherExpense = 1.2};

	ConsumptionRepository* rep = new ConsumptionRepoInMem();
	ConsumptionService ser(rep);
	
	ser.addConsumption(e);
	ser.addConsumption(e);
	double total = ser.getTotal(p);

	qInfo() << total;

	BalanceEntry b = { .type = BalanceType::earning, .description = "Test", .amount = 100, .date = QDate(2000,10,10), .comment = "", .coveringPerson = Person("testName", 1000)};
	BalanceRepoInMem bRep = BalanceRepoInMem();
	bRep.addBalanceEntry(b);
	double totalEarnings = bRep.getTotalEarnings();
	double totalSpendings = bRep.getTotalSpendings();
	
	qInfo() << totalEarnings;
	qInfo() << totalSpendings;






	// GUI Testing

	//QApplication app(argc, argv);

	//CashRegisterSystem rSys;
	//rSys.show();
	//

	//return app.exec();

}