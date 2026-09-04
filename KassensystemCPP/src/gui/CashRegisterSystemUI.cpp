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

#include <QDebug>

CashRegisterSystemUI::CashRegisterSystemUI(const ServiceBundle& services, QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Kassensystem"));
	resize(1000, 600);
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

	tabs = { new PayTab(lowerButtons, services.paymentService, services.personRepo), new AddTab(lowerButtons, services.consumptionService, services.personRepo), new BalanceTab(lowerButtons, services.balanceService, services.personRepo) };

	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::Pay)), tr("Schulden begleichen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::Add)), tr("Einträge hinzufügen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::Balance)), tr("Abteilungsbilanz bearbeiten"));
	
	TabIndex initialTab = TabIndex::Pay; // initialize first tab
	changeTab(initialTab);
	tabSelector->setCurrentIndex(static_cast<int>(initialTab));

	connect(tabSelector, &QTabWidget::currentChanged, this, [this](int idx)
		{
			changeTab(static_cast<TabIndex>(idx));
		});

	connect(lowerButtons.btnCancel, &QPushButton::clicked, qApp, &QCoreApplication::quit);
}

void CashRegisterSystemUI::changeTab(TabIndex activeTab)
{
	int activeTabNum = static_cast<int>(activeTab);

	if (loadedTabs.at(activeTabNum))
	{
		// demanded tab already created
		tabs.at(activeTabNum)->refresh();
	}
	else
	{
		// demanded tab not yet created
		tabs.at(activeTabNum)->initialize();
		loadedTabs.at(activeTabNum) = true;
	}
}