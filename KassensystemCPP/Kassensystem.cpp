#include "Kassensystem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QMainWindow>

CashRegisterSystem::CashRegisterSystem(QMainWindow* parent) : QMainWindow(parent)
{
	setWindowTitle(tr("Kassensystem"));
	resize(800, 600);
	initUi();
}

void CashRegisterSystem::initUi()
{
	// main widget for all contents
	QWidget*		central		= new QWidget(this);
	setCentralWidget(central);
	QVBoxLayout*    rootLayout	= new QVBoxLayout(central);

	// lower buttons
	QHBoxLayout* buttonBar = new QHBoxLayout();
	QPushButton* btnCancel	= new QPushButton("Cancel", this);
	QPushButton* btnApply	= new QPushButton("Apply", this);
	QPushButton* btnSave	= new QPushButton("Save", this);
	buttonBar->addWidget(btnCancel);
	buttonBar->addWidget(btnApply);
	buttonBar->addWidget(btnSave);
	buttonBar->insertStretch(1);			// 1==position, cancel button flushed left, others flushed right
	rootLayout->addLayout(buttonBar);           

	// tabs
	QTabWidget* tabs = new QTabWidget(central);
	rootLayout->insertWidget(0, tabs);

	QWidget* tabAdd = new QWidget();
	QWidget* tabPay = new QWidget();
	QWidget* tabBal = new QWidget();

	tabs->addTab(tabAdd, tr("Einträge hinzufügen"));
	tabs->addTab(tabPay, tr("Schulden begleichen"));
	tabs->addTab(tabBal, tr("Abteilungsbilanz bearbeiten"));

	activeTab = activeTab::activeTabAdd;
	initializeTab(tabs, activeTab);

	connect(tabs, &QTabWidget::currentChanged, this, [tabs, this]()
		{
			this->activeTab = tabs->currentIndex();
			this->initializeTab(tabs, this->activeTab);
		});
}

void CashRegisterSystem::initializeTab(QTabWidget* tabs, int tabNum)
{
	// demanded tab already created
	if (loadedTabs.at(tabNum)) return;

	// demanded tab not yet created
	QWidget* tab = tabs->currentWidget();
	switch(tabNum)
	{
		case activeTab::activeTabAdd:
		{
			//...
			break;

			//QHBoxLayout* entryLayout = new QHBoxLayout(page1);
			//QPushButton* btnTest = new QPushButton("Test", page1);
			//entryLayout->addWidget(btnTest);
			//entryLayout->addStretch();

		}
		case activeTab::activeTabPay:
		{
			//...
			break;
		}		
		case activeTab::activeTabBalance:
		{
			//...
			break;
		}
	}
	loadedTabs.at(tabNum) = true;
}