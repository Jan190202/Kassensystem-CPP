#include "CashRegisterSystemUI.h"
#include "AddTab.h"
#include "PayTab.h"
#include "BalanceTab.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QMainWindow>

CashRegisterSystemUI::CashRegisterSystemUI(const ServiceBundle& services, QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Kassensystem"));
	resize(800, 600);
	initUi(services);
}

void CashRegisterSystemUI::initUi(const ServiceBundle& services)
{
	//main widget for all contents
	QWidget*		central		= new QWidget(this);
	setCentralWidget(central);
	QVBoxLayout*    rootLayout	= new QVBoxLayout(central);

	// lower buttons
	QHBoxLayout* buttonBar = new QHBoxLayout();
	lowerButtons.btnCancel = new QPushButton("Cancel", this);
	lowerButtons.btnApply = new QPushButton("Apply", this);
	lowerButtons.btnSave = new QPushButton("Save", this);
	buttonBar->addWidget(lowerButtons.btnCancel);
	buttonBar->addWidget(lowerButtons.btnApply);
	buttonBar->addWidget(lowerButtons.btnSave);
	buttonBar->insertStretch(1);			// 1==position, cancel button flushed left, others flushed right
	rootLayout->addLayout(buttonBar);           

	//tabs
	QTabWidget* tabSelector = new QTabWidget(central);
	rootLayout->insertWidget(0, tabSelector);

	tabs = { new PayTab(lowerButtons, services.paymentService), new AddTab(lowerButtons, services.consumptionService), new BalanceTab(lowerButtons, services.balanceService) };

	tabSelector->addTab(tabs.at(static_cast<int>(tabIndex::pay)), tr("Schulden begleichen"));
	tabSelector->addTab(tabs.at(static_cast<int>(tabIndex::add)), tr("Einträge hinzufügen"));
	tabSelector->addTab(tabs.at(static_cast<int>(tabIndex::balance)), tr("Abteilungsbilanz bearbeiten"));
	

	activeTab = static_cast<int>(tabIndex::pay); // initialize first tab
	initializeTab(activeTab);
	tabSelector->setCurrentIndex(activeTab);


	connect(tabSelector, &QTabWidget::currentChanged, this, [tabSelector, this]()
		{
			activeTab = tabSelector->currentIndex();
			initializeTab(activeTab);
		});
	connect(lowerButtons.btnCancel, &QPushButton::clicked, qApp, &QCoreApplication::quit);
}

void CashRegisterSystemUI::initializeTab(int tabNum)
{
	// demanded tab already created
	if (loadedTabs.at(tabNum)) return;

	// demanded tab not yet created
	tabs.at(tabNum)->initialize();
	loadedTabs.at(tabNum) = true;
}