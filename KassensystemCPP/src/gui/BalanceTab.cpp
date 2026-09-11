#include "BalanceTab.h"
#include "BalanceTabDialog.h"
#include "GuiTypes.h"
#include "qtutils/QtConversions.h"

#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QTableWidget>
#include <QVBoxLayout>
#include <string>
#include <QInputDialog>

#include <QDebug>

BalanceTab::BalanceTab(const LowerButtonBundle& lowerButtons, BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent) 
	: lowerButtons(lowerButtons), balanceService(balanceService), personRepo(personRepo), BaseTab(parent) {}

void BalanceTab::initialize()
{
	// buttons
	auto* btnAddEarning		= new QPushButton(QStringLiteral("+ Einnahme"), this);
	auto* btnAddSpending	= new QPushButton(QStringLiteral("+ Ausgabe"), this);

	// tables
	tblEarnings		= new QTableWidget(this);
	tblSpendings	= new QTableWidget(this);

	tblEarnings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tblSpendings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// labels
	lCashBefore			= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	lCashDifference		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	lCashAfter			= new QLabel(QtUtils::toCurrencyFormat(0.0), this);

	lSavingsBefore		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	lSavingsDifference	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	lSavingsAfter		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);

	lForeignBefore		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	lForeignAfter		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);

	lEarnings			= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	lSpendings			= new QLabel(QtUtils::toCurrencyFormat(0.0), this);

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
	configureAmount(lForeignBefore);
	configureAmount(lForeignAfter);
	configureAmount(lEarnings);
	configureAmount(lSpendings);

	// earnings
	auto* earningsBox = new QGroupBox(QStringLiteral("Einnahmen"), this);
	auto* earningsLayout = new QVBoxLayout(earningsBox);
	earningsLayout->setContentsMargins(12, 16, 12, 12);
	earningsLayout->setSpacing(10);

	auto* earningsFooterLayout = new QHBoxLayout();
	earningsFooterLayout->setContentsMargins(0, 0, 0, 0);
	earningsFooterLayout->addWidget(new QLabel(QStringLiteral("Gesamt:"), earningsBox));
	earningsFooterLayout->addWidget(lEarnings);
	earningsFooterLayout->addStretch();
	earningsFooterLayout->addWidget(btnAddEarning);

	earningsLayout->addWidget(tblEarnings, 1);
	earningsLayout->addLayout(earningsFooterLayout);

	// spendings
	auto* spendingsBox = new QGroupBox(QStringLiteral("Ausgaben"), this);
	auto* spendingsLayout = new QVBoxLayout(spendingsBox);
	spendingsLayout->setContentsMargins(12, 16, 12, 12);
	spendingsLayout->setSpacing(10);

	auto* spendingsFooterLayout = new QHBoxLayout();
	spendingsFooterLayout->setContentsMargins(0, 0, 0, 0);
	spendingsFooterLayout->addWidget(new QLabel(QStringLiteral("Gesamt:"), spendingsBox));
	spendingsFooterLayout->addWidget(lSpendings);
	spendingsFooterLayout->addStretch();
	spendingsFooterLayout->addWidget(btnAddSpending);

	spendingsLayout->addWidget(tblSpendings, 1);
	spendingsLayout->addLayout(spendingsFooterLayout);

	// before
	beforeBox = new QGroupBox(this);
	
	auto* beforeLayout = new QFormLayout(beforeBox);
	beforeLayout->setContentsMargins(12, 16, 12, 12);
	beforeLayout->setHorizontalSpacing(16);
	beforeLayout->setVerticalSpacing(7);
	beforeLayout->addRow(QStringLiteral("Bestand:"), lSavingsBefore);
	beforeLayout->addRow(QStringLiteral("Bar:"), lCashBefore);
	beforeLayout->addRow(QStringLiteral("davon Fremdanteil:"), lForeignBefore);

	// difference
	auto* differenceBox = new QGroupBox(QStringLiteral("Differenz"), this);

	auto* differenceLayout = new QFormLayout(differenceBox);
	differenceLayout->setContentsMargins(12, 16, 12, 12);
	differenceLayout->setHorizontalSpacing(16);
	differenceLayout->setVerticalSpacing(7);
	differenceLayout->addRow(QStringLiteral("Bestand:"), lSavingsDifference);
	differenceLayout->addRow(QStringLiteral("Bar:"), lCashDifference);

	// after
	auto* btnSettleForeign = new QPushButton("Refresh");

	afterBox = new QGroupBox(this);
	afterBox->setTitle(formatHeader(QDate()));

	auto* afterLayout = new QFormLayout(afterBox);
	afterLayout->setContentsMargins(12, 16, 12, 12);
	afterLayout->setHorizontalSpacing(16);
	afterLayout->setVerticalSpacing(7);
	afterLayout->addRow(QStringLiteral("Bestand:"), lSavingsAfter);
	afterLayout->addRow(QStringLiteral("Bar:"), lCashAfter);
	
	auto* foreignAfterLayout = new QHBoxLayout();
	foreignAfterLayout->setContentsMargins(0, 0, 0, 0);
	foreignAfterLayout->setSpacing(8);
	foreignAfterLayout->addWidget(lForeignAfter);
	foreignAfterLayout->addWidget(btnSettleForeign);
	afterLayout->addRow(QStringLiteral("davon Fremdanteil:"), foreignAfterLayout);

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

	refresh();

	connect(btnAddEarning,  &QPushButton::clicked, this, [=]() {BalanceTab::addEntry(BtnIndex::AddEarning); });
	connect(btnAddSpending, &QPushButton::clicked, this, [=]() {BalanceTab::addEntry(BtnIndex::AddSpending); });
	connect(btnSettleForeign, &QPushButton::clicked, this, [=]() {BalanceTab::addSettlement(); });
}

void BalanceTab::addEntry(BtnIndex mode)
{
	dlgInputs inputs;

	std::vector<entry::Person> personVec = personRepo->getAllPersonEntries();
	auto* inputDialog = new BalanceTabDialog(mode, personVec, this);
	if (inputDialog->exec() == QDialog::Accepted)
	{
		// inputs given and OK pressed
		inputs = inputDialog->getInputs();
		qInfo() << inputs.description;
		qInfo() << inputs.coveringPersonID.has_value();
		if (inputs.coveringPersonID.has_value())
		{
			qInfo() << inputs.coveringPersonID.value();
		}
		qInfo() << inputs.comment;
	}
	else { return; } // cancel pressed

	balanceService.addEntry(
		request::Balance{
			.type = mode==BtnIndex::AddEarning ? BalanceType::Earning : BalanceType::Spending,
			.description = inputs.description,
			.amount = inputs.amount,
			.date = inputs.date,
			.comment = inputs.comment,
			.coveringPersonID = inputs.coveringPersonID
		});

	refresh();
} 

void BalanceTab::refresh()
{
	const registerFinancials::Report report = balanceService.getReport();
	
	refreshTables();
	refreshLables(report);
}

void BalanceTab::refreshTables() const
{
	using TableAllocation = std::pair<QTableWidget*, BalanceType>;
	std::vector<TableAllocation> allocVec;
	allocVec.reserve(2);

	allocVec.emplace_back(tblEarnings, BalanceType::EarningAndSupplement );
	allocVec.emplace_back(tblSpendings, BalanceType::Spending );
	
	
	// populate both tables with entries saved in balanceRepo
	for (size_t i = 0; i < allocVec.size(); i++)
	{
		QTableWidget* table = allocVec.at(i).first;
		BalanceType type = allocVec.at(i).second;

		table->clearContents();
		std::vector<entry::Balance> bEntries = balanceService.getEntries(type);
		int rowCount = bEntries.size();
		int colCount = 3; // description, amount, dateBooked

		table->setRowCount(rowCount);
		table->setColumnCount(colCount);
		table->setHorizontalHeaderLabels(QtUtils::strVecToQStrList({ "Beschreibung", "Betrag (" + Utils::eurSymbol() + ")", "Datum"}));

		for (size_t row = 0; row < bEntries.size(); row++)
		{
			const entry::Balance& bEntry = bEntries.at(row);

			QTableWidgetItem* descriptionItem = new QTableWidgetItem(QString::fromStdString(bEntry.description));
			QTableWidgetItem* amountItem = new QTableWidgetItem(QString::number(bEntry.amount, 'f', 2));
			QTableWidgetItem* dateItem = new QTableWidgetItem(bEntry.dateBooked.toString(QStringLiteral("dd.MM.yyyy")));

			//descriptionItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//amountItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//dateItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

			table->setItem(row, 0, descriptionItem);
			table->setItem(row, 1, amountItem);
			table->setItem(row, 2, dateItem);
		}

		table->resizeColumnsToContents();
	}
}

void BalanceTab::refreshLables(const registerFinancials::Report& report) const
{
	lEarnings->setText(QtUtils::toCurrencyFormat(report.totalEarnings));
	lSpendings->setText(QtUtils::toCurrencyFormat(report.totalSpendings));

	beforeBox->setTitle(formatHeader(report.stateBefore.date));
	lCashBefore->setText(QtUtils::toCurrencyFormat(report.stateBefore.cash));
	lSavingsBefore->setText(QtUtils::toCurrencyFormat(report.stateBefore.savings));
	lForeignBefore->setText(QtUtils::toCurrencyFormat(report.stateBefore.foreignCash));

	lSavingsDifference->setText(QtUtils::toCurrencyFormat(report.savingsDiff));
	lCashDifference->setText(QtUtils::toCurrencyFormat(report.cashDiff));

	afterBox->setTitle(formatHeader(report.stateAfter.date));
	lSavingsAfter->setText(QtUtils::toCurrencyFormat(report.stateAfter.savings));
	lCashAfter->setText(QtUtils::toCurrencyFormat(report.stateAfter.cash));
	lForeignAfter->setText(QtUtils::toCurrencyFormat(report.stateAfter.foreignCash, 3));
}

void BalanceTab::apply()
{
	// TBD
}

void BalanceTab::save()
{
	apply();
	// TBD
}

QString BalanceTab::formatHeader(const QDate& date) const
{
	return QStringLiteral("Stand %1").arg(date.toString(QStringLiteral("dd.MM.yyyy")));
}

void BalanceTab::addSettlement()
{
	const double minValue = 0.0;
	const double maxValue = balanceService.getReport().stateAfter.foreignCash; // TBD: get directly from repo
	const double initValue = maxValue;
	const int decimals = 2;

	QInputDialog dialog(this);
	dialog.setWindowTitle(QStringLiteral("Fremdanteil begleichen"));
	dialog.setLabelText(QString::fromStdString("Betrag (" + Utils::eurSymbol() + "):"));

	dialog.setDoubleRange(minValue, maxValue);
	dialog.setDoubleDecimals(decimals);
	dialog.setDoubleValue(initValue);

	QSize size = dialog.sizeHint();
	size.setWidth(qMax(size.width(), 200));
	dialog.resize(size);

	if (dialog.exec() != QDialog::Accepted)
		return;

	const double settledAmount = dialog.doubleValue();

	if (settledAmount == 0)
		return;

	auto returnMsg = balanceService.addShareSettlement(
		request::ShareSettlement{
			.amount = settledAmount
		});

	switch (returnMsg) // currently not needed; can be used for error presentation like an error dialog if needed
	{
	case AddSettlementException::None: break;
	case AddSettlementException::AmountZero: break;
	case AddSettlementException::AmountNegative: break;
	case AddSettlementException::AmountGreaterThanTotalForeignShare: break;
	}

	refresh();
}