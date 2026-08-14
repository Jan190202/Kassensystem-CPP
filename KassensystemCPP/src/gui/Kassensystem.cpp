#include "Kassensystem.h"
#include "AddTab.h"
#include "PayTab.h"
#include "BalanceTab.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QMainWindow>

CashRegisterSystem::CashRegisterSystem(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Kassensystem"));
	resize(800, 600);
	initUi();
}

void CashRegisterSystem::initUi()
{
	//main widget for all contents
	QWidget*		central		= new QWidget(this);
	setCentralWidget(central);
	QVBoxLayout*    rootLayout	= new QVBoxLayout(central);

	// lower buttons
	QHBoxLayout* buttonBar = new QHBoxLayout();
	btnCancel = new QPushButton("Cancel", this);
	btnApply = new QPushButton("Apply", this);
	btnSave = new QPushButton("Save", this);
	buttonBar->addWidget(btnCancel);
	buttonBar->addWidget(btnApply);
	buttonBar->addWidget(btnSave);
	buttonBar->insertStretch(1);			// 1==position, cancel button flushed left, others flushed right
	rootLayout->addLayout(buttonBar);           

	//tabs
	QTabWidget* tabSelector = new QTabWidget(central);
	rootLayout->insertWidget(0, tabSelector);

	tabs = { new PayTab(), new AddTab(), new BalanceTab() };

	tabSelector->addTab(tabs.at(0), tr("Schulden begleichen"));
	tabSelector->addTab(tabs.at(1), tr("Einträge hinzufügen"));
	tabSelector->addTab(tabs.at(2), tr("Abteilungsbilanz bearbeiten"));
	

	activeTab = static_cast<int>(tabIndex::balance); // initialize first tab
	initializeTab(activeTab);
	tabSelector->setCurrentIndex(activeTab);


	connect(tabSelector, &QTabWidget::currentChanged, this, [tabSelector, this]()
		{
			activeTab = tabSelector->currentIndex();
			initializeTab(activeTab);
		});
}

void CashRegisterSystem::initializeTab(int tabNum)
{
	// demanded tab already created
	if (loadedTabs.at(tabNum)) return;

	// demanded tab not yet created
	tabs.at(tabNum)->initialize();
	loadedTabs.at(tabNum) = true;
}