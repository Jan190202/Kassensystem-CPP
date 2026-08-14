#include "BalanceTab.h"

#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDate>
#include <QFrame>

BalanceTab::BalanceTab(QWidget* parent) : BaseTab(parent) {}

void BalanceTab::initialize()
{
	// temporary
	dateBefore = QDate(2000, 1, 1);

	// buttons
	auto* btnAddSpending	= new QPushButton(tr("+"), this);
	auto* btnAddEarning		= new QPushButton(tr("+"), this);

	// tables
	tblEarnings		= new QTableView(this);
	tblSpendings	= new QTableView(this);

	// labels
	lCashBefore			= new QLabel(tr("0.00 €"));
	lCashDifference		= new QLabel(tr("0.00 €"));
	lCashAfter			= new QLabel(tr("0.00 €"));

	lSavingsBefore		= new QLabel(tr("0.00 €"));
	lSavingsDifference	= new QLabel(tr("0.00 €"));
	lSavingsAfter		= new QLabel(tr("0.00 €"));

	lEarnings			= new QLabel(tr("0.00 €"));
	lSpendings			= new QLabel(tr("0.00 €"));

	// layouting
	auto* mainLayout				= new QVBoxLayout(this);
	auto* tableLayout				= new QHBoxLayout();
	auto* tableEarningsLayout		= new QVBoxLayout();
	auto* tableEarningsLowerLayout	= new QHBoxLayout();
	auto* tableSpendingsLayout		= new QVBoxLayout();
	auto* tableSpendingsLowerLayout	= new QHBoxLayout();
	auto* summaryLayout				= new QHBoxLayout();
	auto* summaryBeforeLayout		= new QGridLayout();
	auto* summaryDifferenceLayout	= new QGridLayout();
	auto* summaryAfterLayout		= new QGridLayout();


	mainLayout->addLayout(tableLayout);
	tableLayout->addLayout(tableEarningsLayout);
	auto* vLine = new QFrame(); vLine->setFrameShape(QFrame::VLine);
	tableLayout->addWidget(vLine);
	tableLayout->addLayout(tableSpendingsLayout);
	auto* hLine = new QFrame(); hLine->setFrameShape(QFrame::HLine);
	mainLayout->addWidget(hLine);
	mainLayout->addLayout(summaryLayout);
	summaryLayout->addLayout(summaryBeforeLayout);
	summaryLayout->addLayout(summaryDifferenceLayout);
	summaryLayout->addLayout(summaryAfterLayout);
	
	tableEarningsLayout->addWidget(new QLabel(tr("Einnahmen")));
	tableEarningsLayout->addWidget(tblEarnings);
	tableEarningsLayout->addLayout(tableEarningsLowerLayout);
	tableEarningsLowerLayout->addWidget(new QLabel(tr("Gesamt:")));
	tableEarningsLowerLayout->addWidget(lEarnings);
	tableEarningsLowerLayout->addWidget(btnAddEarning);

	tableSpendingsLayout->addWidget(new QLabel(tr("Ausgaben")));
	tableSpendingsLayout->addWidget(tblSpendings);
	tableSpendingsLayout->addLayout(tableSpendingsLowerLayout);
	tableSpendingsLowerLayout->addWidget(new QLabel(tr("Gesamt:")));
	tableSpendingsLowerLayout->addWidget(lSpendings);
	tableSpendingsLowerLayout->addWidget(btnAddSpending);

	summaryBeforeLayout->addWidget(new QLabel(tr("Stand ") + dateBefore.toString("dd.MM.yyyy")), 0, 0, 1, 2);
	summaryBeforeLayout->addWidget(new QLabel(tr("Bestand:")),						1, 0, 1, 1);
	summaryBeforeLayout->addWidget(lSavingsBefore,									1, 1, 1, 1);
	summaryBeforeLayout->addWidget(new QLabel(tr("Bar:")),							2, 0, 1, 1);
	summaryBeforeLayout->addWidget(lCashBefore,										2, 1, 1, 1);
	
	summaryDifferenceLayout->addWidget(new QLabel(tr("Differenz")),					0, 0, 1, 2);
	summaryDifferenceLayout->addWidget(new QLabel(tr("Bestand:")),					1, 0, 1, 1);
	summaryDifferenceLayout->addWidget(lSavingsDifference,							1, 1, 1, 1);
	summaryDifferenceLayout->addWidget(new QLabel(tr("Bar:")),						2, 0, 1, 1);
	summaryDifferenceLayout->addWidget(lCashDifference,								2, 1, 1, 1);


	summaryAfterLayout->addWidget(new QLabel(tr("Stand ") + dateAfter.toString("dd.MM.yyyy")),	0, 0, 1, 2);
	summaryAfterLayout->addWidget(new QLabel(tr("Bestand:")),						1, 0, 1, 1);
	summaryAfterLayout->addWidget(lSavingsAfter,									1, 1, 1, 1);
	summaryAfterLayout->addWidget(new QLabel(tr("Bar:")),							2, 0, 1, 1);
	summaryAfterLayout->addWidget(lCashAfter,										2, 1, 1, 1);

}