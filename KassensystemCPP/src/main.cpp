#include "gui/Kassensystem.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	CashRegisterSystem rSys;
	rSys.show();
	

	return app.exec();
}