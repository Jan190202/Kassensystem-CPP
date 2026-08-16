#include "BalanceTab.h"
#include "BalanceTabDialog.h"
#include "commons/CommonTypes.h";

#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QTableView>
#include <QVBoxLayout>
#include <string>

#include <QDebug>

BalanceTab::BalanceTab(QWidget* parent) : BaseTab(parent) {}

void BalanceTab::initialize()
{
	// Temporary
	dateBefore = QDate(2000, 1, 1);

	// Buttons
	auto* btnAddEarning		= new QPushButton(tr("+ Einnahme"), this);
	auto* btnAddSpending	= new QPushButton(tr("+ Ausgabe"), this);

	// Tables
	tblEarnings		= new QTableView(this);
	tblSpendings	= new QTableView(this);

	tblEarnings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tblSpendings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Labels
	lCashBefore			= new QLabel(tr("0,00 €"), this);
	lCashDifference		= new QLabel(tr("0,00 €"), this);
	lCashAfter			= new QLabel(tr("0,00 €"), this);

	lSavingsBefore		= new QLabel(tr("0,00 €"), this);
	lSavingsDifference	= new QLabel(tr("0,00 €"), this);
	lSavingsAfter		= new QLabel(tr("0,00 €"), this);

	lEarnings			= new QLabel(tr("0,00 €"), this);
	lSpendings			= new QLabel(tr("0,00 €"), this);

	const auto configureAmount = [](QLabel* label)
		{
			label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			label->setMinimumWidth(90);
		};

	configureAmount(lCashBefore);
	configureAmount(lCashDifference);
	configureAmount(lCashAfter);
	configureAmount(lSavingsBefore);
	configureAmount(lSavingsDifference);
	configureAmount(lSavingsAfter);
	configureAmount(lEarnings);
	configureAmount(lSpendings);

	// earnings
	auto* earningsBox = new QGroupBox(tr("Einnahmen"), this);
	auto* earningsLayout = new QVBoxLayout(earningsBox);
	earningsLayout->setContentsMargins(12, 16, 12, 12);
	earningsLayout->setSpacing(10);

	auto* earningsFooterLayout = new QHBoxLayout();
	earningsFooterLayout->setContentsMargins(0, 0, 0, 0);
	earningsFooterLayout->addWidget(new QLabel(tr("Gesamt:"), earningsBox));
	earningsFooterLayout->addWidget(lEarnings);
	earningsFooterLayout->addStretch();
	earningsFooterLayout->addWidget(btnAddEarning);

	earningsLayout->addWidget(tblEarnings, 1);
	earningsLayout->addLayout(earningsFooterLayout);

	// spendings
	auto* spendingsBox = new QGroupBox(tr("Ausgaben"), this);
	auto* spendingsLayout = new QVBoxLayout(spendingsBox);
	spendingsLayout->setContentsMargins(12, 16, 12, 12);
	spendingsLayout->setSpacing(10);

	auto* spendingsFooterLayout = new QHBoxLayout();
	spendingsFooterLayout->setContentsMargins(0, 0, 0, 0);
	spendingsFooterLayout->addWidget(new QLabel(tr("Gesamt:"), spendingsBox));
	spendingsFooterLayout->addWidget(lSpendings);
	spendingsFooterLayout->addStretch();
	spendingsFooterLayout->addWidget(btnAddSpending);

	spendingsLayout->addWidget(tblSpendings, 1);
	spendingsLayout->addLayout(spendingsFooterLayout);

	// before
	auto* beforeBox = new QGroupBox(
		tr("Stand %1").arg(dateBefore.toString(QStringLiteral("dd.MM.yyyy"))),
		this);

	auto* beforeLayout = new QFormLayout(beforeBox);
	beforeLayout->setContentsMargins(12, 16, 12, 12);
	beforeLayout->setHorizontalSpacing(16);
	beforeLayout->setVerticalSpacing(7);
	beforeLayout->addRow(tr("Bestand:"), lSavingsBefore);
	beforeLayout->addRow(tr("Bar:"), lCashBefore);

	// difference
	auto* differenceBox = new QGroupBox(tr("Differenz"), this);

	auto* differenceLayout = new QFormLayout(differenceBox);
	differenceLayout->setContentsMargins(12, 16, 12, 12);
	differenceLayout->setHorizontalSpacing(16);
	differenceLayout->setVerticalSpacing(7);
	differenceLayout->addRow(tr("Bestand:"), lSavingsDifference);
	differenceLayout->addRow(tr("Bar:"), lCashDifference);

	// after
	auto* afterBox = new QGroupBox(
		tr("Stand %1").arg(dateAfter.toString(QStringLiteral("dd.MM.yyyy"))),
		this);

	auto* afterLayout = new QFormLayout(afterBox);
	afterLayout->setContentsMargins(12, 16, 12, 12);
	afterLayout->setHorizontalSpacing(16);
	afterLayout->setVerticalSpacing(7);
	afterLayout->addRow(tr("Bestand:"), lSavingsAfter);
	afterLayout->addRow(tr("Bar:"), lCashAfter);

	// main layout
	auto* tableLayout = new QHBoxLayout();
	tableLayout->setContentsMargins(0, 0, 0, 0);
	tableLayout->setSpacing(14);
	tableLayout->addWidget(earningsBox, 1);
	tableLayout->addWidget(spendingsBox, 1);

	auto* summaryLayout = new QHBoxLayout();
	summaryLayout->setContentsMargins(0, 0, 0, 0);
	summaryLayout->setSpacing(14);
	summaryLayout->addWidget(beforeBox, 1);
	summaryLayout->addWidget(differenceBox, 1);
	summaryLayout->addWidget(afterBox, 1);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(18, 18, 18, 18);
	mainLayout->setSpacing(14);
	mainLayout->addLayout(tableLayout, 1);
	mainLayout->addLayout(summaryLayout);

	connect(btnAddEarning,  &QPushButton::clicked, this, [=]() {BalanceTab::addEntry(btnIndex::addEarning); });
	connect(btnAddSpending, &QPushButton::clicked, this, [=]() {BalanceTab::addEntry(btnIndex::addSpending); });
}

void BalanceTab::addEntry(btnIndex mode)
{
	auto* inputDialog = new BalanceTabDialog(mode, this);
	if (inputDialog->exec() == QDialog::Accepted)
	{
		// inputs given and OK pressed
		dlgInputs inputs = inputDialog->getInputs();
		qInfo() << inputs.description;
		qInfo() << inputs.isCovered;
		qInfo() << inputs.coveringPerson;
		qInfo() << inputs.comment;
	}
	else {} // cancel pressed
} 

void BalanceTab::collectResults()
{
	qInfo() << "CollectResults called";
}