#include "Kassensystem.h"
#include <QApplication.h>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	CashRegisterSystem rSys;
	rSys.show();
	

	return app.exec();
}