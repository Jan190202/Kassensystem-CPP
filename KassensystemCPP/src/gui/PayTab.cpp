#include "PayTab.h"

#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QFrame>

PayTab::PayTab(QWidget* parent) : BaseTab(parent) {}

void PayTab::initialize()
{
	nameSelect = new QComboBox(this);



	totalLabel = new QLabel("0.00€", this);
	paidLabel = new QLabel("0.00€", this);
	dueLabel = new QLabel("0.00€", this);
	creditLabel = new QLabel("0.00€", this);

	consumptionTable = new QTableWidget(10,10,this);
}