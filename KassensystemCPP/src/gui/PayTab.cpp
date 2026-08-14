#include "PayTab.h"

#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>

PayTab::PayTab(QWidget* parent) : BaseTab(parent) {}

void PayTab::initialize()
{
	// name select
	nameSelect = new QComboBox(this);

	// info texts, button for using credit
	QLabel* totalTextLabel = new QLabel("Gesamt:", this);
	QLabel* paidTextLabel = new QLabel("Bezahlt:", this);
	QLabel* dueTextLabel = new QLabel("Ausstehend:", this);
	QLabel* creditTextLabel = new QLabel("Guthaben:", this);

	totalNumLabel = new QLabel("0.00€", this);
	paidNumLabel = new QLabel("0.00€", this);
	dueNumLabel = new QLabel("0.00€", this);
	creditNumLabel = new QLabel("0.00€", this);

	btnUseCredit = new QPushButton("Refresh", this);
	btnUseCredit->setDisabled(true);

	// payment
	paymentSpinBox = new QDoubleSpinBox();
	fullPaymentCheckBox = new QCheckBox("vollständig");

	// surplus usage
	btnSurplusToCredit = new QRadioButton("Guthaben");
	btnSurplusToCredit->setChecked(true);
	btnSurplusToTip = new QRadioButton("Trinkgeld");
	QButtonGroup* surplusGroup = new QButtonGroup();
	surplusGroup->addButton(btnSurplusToCredit);
	surplusGroup->addButton(btnSurplusToTip);
	QGroupBox* surplusBox = new QGroupBox("Überschuss verwenden als");

	// vertical line in middle
	QFrame* vLine = new QFrame();
	vLine->setFrameShape(QFrame::VLine);

	// table
	consumptionTable = new QTableWidget(10,3,this);



	// layouting
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	QGridLayout* infoLayout = new QGridLayout();
	QVBoxLayout* surplusBtnLayout = new QVBoxLayout();

	surplusBtnLayout->addWidget(btnSurplusToCredit);
	surplusBtnLayout->addWidget(btnSurplusToTip);
	surplusBox->setLayout(surplusBtnLayout);

	mainLayout->addLayout(infoLayout, 1);
	mainLayout->addWidget(vLine);
	mainLayout->addWidget(consumptionTable, 3);
	
	infoLayout->addWidget(nameSelect,			0, 0, 1, 3);
	infoLayout->addWidget(totalTextLabel,		1, 0, 1, 1);
	infoLayout->addWidget(totalNumLabel,		1, 1, 1, 1);
	infoLayout->addWidget(paidTextLabel,		2, 0, 1, 1);
	infoLayout->addWidget(paidNumLabel,			2, 1, 1, 1);
	infoLayout->addWidget(dueTextLabel,			3, 0, 1, 1);
	infoLayout->addWidget(dueNumLabel,			3, 1, 1, 1);
	infoLayout->addWidget(creditTextLabel,		4, 0, 1, 1);
	infoLayout->addWidget(creditNumLabel,		4, 1, 1, 1);
	infoLayout->addWidget(btnUseCredit,			4, 2, 1, 1);

	infoLayout->addWidget(paymentSpinBox,		5, 0, 1, 1);
	infoLayout->addWidget(fullPaymentCheckBox,	5, 1, 1, 1);

	infoLayout->addWidget(surplusBox,			6, 0, 1, 2);

	//infoLayout->addWidget(btnSurplusToCredit,	6, 1, 1, 2);
	//infoLayout->addWidget(btnSurplusToTip,		7, 1, 1, 2);
}