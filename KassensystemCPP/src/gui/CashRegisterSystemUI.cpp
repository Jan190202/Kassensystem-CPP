#include "gui/CashRegisterSystemUI.h"
#include "gui/tabs/AddTab.h"
#include "gui/tabs/PayTab.h"
#include "gui/tabs/BalanceTab.h"
#include "gui/tabs/ManualTab.h"
#include "gui/components/TextPopupWidget.h"
#include "gui/IconLoader.h"
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QMenu>
#include <QDebug>
#include <QIcon>

CashRegisterSystemUI::CashRegisterSystemUI(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller, QSqlDatabase& db, PendingChangeLog& log, QWidget* parent) : log(log), QMainWindow(parent)
{
	setWindowTitle(QStringLiteral("Kassensystem"));
	setWindowIcon(iconLoader::getIcon("cash-machine.png", false));
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
	lowerButtons.btnCancel = new QPushButton("Cancel", this);
	lowerButtons.btnApply = new QPushButton("Apply", this);
	lowerButtons.btnSave = new QPushButton("Save && Sync", this);
	lowerButtons.btnInfo = new TextPopupWidget(TextPopupWidget::PopupPos::topLeft, this);
	lowerButtons.btnSave->setEnabled(false); // disabled, until changes were made
	lowerButtons.btnInfo->setEnabled(false); // disabled, until changes were made
	
	QHBoxLayout* buttonBar = new QHBoxLayout();
	buttonBar->addWidget(lowerButtons.btnCancel);
	buttonBar->addStretch();
	buttonBar->addWidget(lowerButtons.btnApply);
	buttonBar->addWidget(lowerButtons.btnSave);
	buttonBar->addWidget(lowerButtons.btnInfo);
	rootLayout->addLayout(buttonBar);           

	//tabs
	QTabWidget* tabSelector = new QTabWidget(central);
	rootLayout->insertWidget(0, tabSelector);

	tabs = { 
		new PayTab(serviceBundle.paymentService, serviceBundle.personService, repoBundle.personRepo, repoBundle.consumptionRepo, repoBundle.debtRepo, repoBundle.creditRepo),
		new AddTab(serviceBundle.consumptionService, repoBundle.personRepo), 
		new BalanceTab(serviceBundle.balanceService, repoBundle.personRepo),
		new ManualTab(repoBundle, db, log) };

	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::pay)), QStringLiteral("Schulden begleichen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::add)), QStringLiteral("Einträge hinzufügen"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::balance)), QStringLiteral("Abteilungsbilanz bearbeiten"));
	tabSelector->addTab(tabs.at(static_cast<int>(TabIndex::manual)), QStringLiteral("Manuelle Anpassung"));
	
	changeTab(activeTab);
	tabSelector->setCurrentIndex(static_cast<int>(activeTab));

	connect(tabSelector, &QTabWidget::currentChanged, this, [&, tabSelector](int idx)
		{
			if (activeTabHasTemporaryChanges)
				if (QMessageBox::question(this, tr("Tab-Wechsel"), tr("Änderungen verwerfen?")) != QMessageBox::Yes)
				{
					QSignalBlocker blocker(tabSelector);
					tabSelector->setCurrentIndex(static_cast<int>(activeTab));
					return;
				}

			activeTab = static_cast<TabIndex>(idx);
			changeTab(activeTab);

			qDebug() << ""; // add new line for easier debugging
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
			refreshButtonBar();
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

	activeTabHasTemporaryChanges = false;
	refreshButtonBar();



	// apply button only controls the active tab
	lowerButtons.btnApply->disconnect();
	connect(lowerButtons.btnApply, &QPushButton::clicked, this, [=]()
		{
			tabs.at(activeTabNum)->apply();
			refreshButtonBar();
		});

	tabs.at(activeTabNum)->disconnect();
	connect(tabs.at(activeTabNum), &BaseTab::temporaryChangesExist, this, [=](bool doExist)
		{
			activeTabHasTemporaryChanges = doExist;
			refreshButtonBar();
		});

	connect(tabs.at(activeTabNum), &BaseTab::instantChangesMade, this, [=]()
		{
			refreshButtonBar();
		});
}

void CashRegisterSystemUI::refreshButtonBar()
{
	lowerButtons.btnApply->setEnabled(activeTabHasTemporaryChanges);
	lowerButtons.btnSave->setEnabled(log.hasPendingChanges());
	lowerButtons.btnInfo->setEnabled(log.hasPendingChanges());
	lowerButtons.btnInfo->setRichText(QString::fromStdString(log.printPendingChanges(PendingChangeLog::TextFormat::rich)));
}