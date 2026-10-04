#include "CashRegisterSystemUI.h"
#include "AddTab.h"
#include "PayTab.h"
#include "BalanceTab.h"
#include "ManualTab.h"
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QDebug>

CashRegisterSystemUI::CashRegisterSystemUI(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller, QSqlDatabase& db, PendingChangeLog& log, QWidget* parent) : log(log), QMainWindow(parent)
{
	setWindowTitle(QStringLiteral("Kassensystem"));
	resize(1000, 600);
	initUi(serviceBundle, repoBundle, controller, db);
}

void CashRegisterSystemUI::initUi(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller, QSqlDatabase& db)
{
	//main widget for all contents
	QWidget*		central		= new QWidget(this);
	setCentralWidget(central);
	QVBoxLayout*    rootLayout	= new QVBoxLayout(central);

	// lower buttons
	QHBoxLayout* buttonBar = new QHBoxLayout();
	lowerButtons.btnCancel = new QPushButton("Cancel", this);
	lowerButtons.btnApply = new QPushButton("Apply", this);
	lowerButtons.btnSave = new QPushButton("Save && Sync", this);
	lowerButtons.btnSave->setEnabled(false); // disabled, until changes were made
	
	buttonBar->addWidget(lowerButtons.btnCancel);
	buttonBar->addWidget(lowerButtons.btnApply);
	buttonBar->addWidget(lowerButtons.btnSave);
	buttonBar->insertStretch(1);			// 1==position, cancel button flushed left, others flushed right
	rootLayout->addLayout(buttonBar);           

	//tabs
	QTabWidget* tabSelector = new QTabWidget(central);
	rootLayout->insertWidget(0, tabSelector);

	tabs = { 
		new PayTab(lowerButtons, serviceBundle.paymentService, serviceBundle.personService, repoBundle.personRepo, repoBundle.consumptionRepo, repoBundle.debtRepo, repoBundle.creditRepo),
		new AddTab(lowerButtons, serviceBundle.consumptionService, repoBundle.personRepo), 
		new BalanceTab(lowerButtons, serviceBundle.balanceService, repoBundle.personRepo),
		new ManualTab(lowerButtons, repoBundle, db, log) };

	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::pay)), QStringLiteral("Schulden begleichen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::add)), QStringLiteral("Einträge hinzufügen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::balance)), QStringLiteral("Abteilungsbilanz bearbeiten"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::manual)), QStringLiteral("Manuelle Anpassung"));
	
	TabIndex initialTab = TabIndex::pay; // initialize first tab
	changeTab(initialTab);
	tabSelector->setCurrentIndex(static_cast<int>(initialTab));

	connect(tabSelector, &QTabWidget::currentChanged, this, [this](int idx)
		{
			qDebug() << ""; // add new line for easier debugging
			changeTab(static_cast<TabIndex>(idx));
		});

	connect(lowerButtons.btnCancel, &QPushButton::clicked, this, [&]() 
		{
			controller.close(); 
			qApp->quit(); 
		});

	connect(lowerButtons.btnSave, &QPushButton::clicked, this, [&]() 
		{
			controller.save();
			controller.sync();
			log.clear();
			lowerButtons.btnSave->setEnabled(false);
		});
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

	// apply button only controls the active tab
	lowerButtons.btnApply->disconnect();
	connect(lowerButtons.btnApply, &QPushButton::clicked, this, [=]()
		{
			tabs.at(activeTabNum)->apply();
			lowerButtons.btnSave->setEnabled(log.hasPendingChanges());
		});
}