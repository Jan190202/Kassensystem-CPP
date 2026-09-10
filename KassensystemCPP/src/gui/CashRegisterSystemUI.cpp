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

CashRegisterSystemUI::CashRegisterSystemUI(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle(QStringLiteral("Kassensystem"));
	resize(1000, 600);
	initUi(serviceBundle, repoBundle);
}

void CashRegisterSystemUI::initUi(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle)
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

	tabs = { 
		new PayTab(lowerButtons, serviceBundle.paymentService, repoBundle.personRepo), 
		new AddTab(lowerButtons, serviceBundle.consumptionService, repoBundle.personRepo), 
		new BalanceTab(lowerButtons, serviceBundle.balanceService, repoBundle.personRepo) };

	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::Pay)), QStringLiteral("Schulden begleichen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::Add)), QStringLiteral("Einträge hinzufügen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::Balance)), QStringLiteral("Abteilungsbilanz bearbeiten"));
	
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