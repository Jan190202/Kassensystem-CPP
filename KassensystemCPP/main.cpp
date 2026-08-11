# include "Kassensystem.h"
# include <qapplication.h>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	CashRegisterSystem uiApp;
	uiApp.show();

	return app.exec();
}