#include "gui/tabs/BalanceTab.h"
#include "gui/dialogs/BalanceTabAddEntryDialog.h"
#include "gui/dialogs/BalanceTabSettlementDialog.h"
#include "gui/types/GuiTypes.h"
#include "qtutils/QtConversions.h"
#include "qtutils/InstantToolTip.h"
#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QDebug>
#include <string>
#include <algorithm>
#include <cmath>

BalanceTab::BalanceTab(BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent) 
	: balanceService(balanceService), personRepo(personRepo), BaseTab(parent) {}

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
	lCashBefore			= new QLabel(qtUtils::toCurrencyFormat(0.0), this);
	lCashDifference		= new QLabel(qtUtils::toCurrencyFormat(0.0), this);
	lCashAfter			= new QLabel(qtUtils::toCurrencyFormat(0.0), this);

	lSavingsBefore		= new QLabel(qtUtils::toCurrencyFormat(0.0,3), this);
	lSavingsDifference	= new QLabel(qtUtils::toCurrencyFormat(0.0,3), this);
	lSavingsAfter		= new QLabel(qtUtils::toCurrencyFormat(0.0,3), this);

	lForeignBefore		= new QLabel(qtUtils::toCurrencyFormat(0.0,3), this);
	lForeignAfter		= new QLabel(qtUtils::toCurrencyFormat(0.0,3), this);

	lEarnings			= new QLabel(qtUtils::toCurrencyFormat(0.0,3), this);
	lSpendings			= new QLabel(qtUtils::toCurrencyFormat(0.0), this);

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

	connect(btnAddEarning,  &QPushButton::clicked, this, [=]() {BalanceTab::addEntry(BtnIndex::addEarning); });
	connect(btnAddSpending, &QPushButton::clicked, this, [=]() {BalanceTab::addEntry(BtnIndex::addSpending); });
	connect(btnSettleForeign, &QPushButton::clicked, this, [=]() {BalanceTab::addSettlement(); });
}

void BalanceTab::addEntry(BtnIndex mode)
{
	BalanceTabAddEntryDialog::inputs inputs;

	std::vector<entry::Person> personVec = personRepo->getAllPersonEntries();
	auto* inputDialog = new BalanceTabAddEntryDialog(mode, personVec, this);
	if (inputDialog->exec() == QDialog::Accepted)
	{
		// inputs given and OK pressed
		inputs = inputDialog->getInputs();
	}
	else { return; } // cancel pressed

	balanceService.addBalanceItem(
		request::Balance{
			.type = mode==BtnIndex::addEarning ? BalanceType::earning : BalanceType::spending,
			.description = inputs.description,
			.amount = inputs.amount,
			.dateBooked = inputs.date,
			.comment = inputs.comment,
			.coveringPerson = inputs.coveringPerson
		});

	Q_EMIT instantChangesMade();

	refresh();
} 

void BalanceTab::refresh()
{
	report = balanceService.getReport();
	
	refreshTables(report);
	refreshLables(report);
}

void BalanceTab::refreshTables(const registerFinancials::Report& report) const
{
	using TableAllocation = std::pair<QTableWidget*, BalanceType>;
	std::vector<TableAllocation> allocVec;
	allocVec.reserve(2);

	allocVec.emplace_back(tblEarnings, BalanceType::earning | BalanceType::supplement );
	allocVec.emplace_back(tblSpendings, BalanceType::spending );
	
	
	// populate both tables with entries saved in balanceRepo
	for (size_t i = 0; i < allocVec.size(); i++)
	{
		QTableWidget* table = allocVec.at(i).first;
		BalanceType type = allocVec.at(i).second;

		table->clearContents();
		std::vector<entry::Balance> bEntries = balanceService.getBalanceEntries(type, report.stateBefore.date);

		std::sort(bEntries.begin(), bEntries.end(), [](const entry::Balance& a, const entry::Balance& b) 
			{
				return a.dateBooked > b.dateBooked;
			});

		int rowCount = bEntries.size();
		int colCount = 3; // description, amount, dateBooked

		table->setRowCount(rowCount);
		table->setColumnCount(colCount);
		table->setHorizontalHeaderLabels(qtUtils::strVecToQStrList({ "Beschreibung", "Betrag (" + utils::eurSymbol() + ")", "Datum"}));

		for (size_t row = 0; row < bEntries.size(); row++)
		{
			const entry::Balance& bEntry = bEntries.at(row);

			QTableWidgetItem* descriptionItem = new QTableWidgetItem(QString::fromStdString(bEntry.description));
			QTableWidgetItem* amountItem = new QTableWidgetItem(QString::number(bEntry.amount, 'f', (hasFlag(bEntry.type, BalanceType::supplement) ? 3 : 2)));
			QTableWidgetItem* dateItem = new QTableWidgetItem(bEntry.dateBooked.toQString());

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
	auto row = [&](bool addsPositively, double num, QString desc, bool isLast, double decimals = 2) -> QString
		{
			QString color = addsPositively ? "#2e8b57" : "#c0392b"; // green / red
			QString pre = addsPositively ? "+" : "-";
			QString val = num >= 0 ? qtUtils::toCurrencyFormat(num, decimals) : ("(" + qtUtils::toCurrencyFormat(num, decimals) + ")");

			return QString(
				"<tr>"
				"<td style=\"color:%1; font-weight:bold; padding-right:4px;\">%2</td>"
				"<td align=\"right\" style=\"font-weight:bold; padding-right:8px;\">%3</td>"
				"<td>%4</td>"
				"</tr>"
			).arg(color, pre, val, desc); // QString supports rich text, HTML-formatted
		};

	auto& d = report.details;

	// tooltip explanation for the cash difference
	QString cashDiffExplanation =
		"<table cellspacing=\"2\" cellpadding=\"0\">" +
		row(true, d.departmentEarnings, "Einnahmen (ohne Verkäufe)", false) +
		row(false, d.departmentSpendings, "Ausgaben", false) +
		row(true, d.paidDebt, "bezahlte Verbräuche", false) +
		row(false, d.settledValue, "85%-Abgabe", false) +
		row(true, d.depositedCredit, "Guthaben", true) +
		"</table>";

	// tooltip explanation for the current savings
	const double cashAfter = report.stateAfter.cash;
	const double foreignAfter = report.stateAfter.foreignCash;
	const double debtAllTime = report.details.consumptionAllSharesAllTime - report.details.paidDebtAllTime;
	const double creditAllTime = report.details.depositedCreditAllTime;
	const double expectedSavings = report.stateAfter.savings;

	const double savingsSum = cashAfter - foreignAfter + debtAllTime - creditAllTime;
	const bool savingsMatch = std::abs(savingsSum - expectedSavings) < 1e-6;

	const QString checkColor = savingsMatch ? "#2e8b57" : "#c0392b";
	const QString checkText = savingsMatch ? "&#10004; korrekt"	: "&#10008; inkorrekt";

	QString sumRow = QString(
		"<tr><td colspan=\"3\"><hr></td></tr>"
		"<tr>"
		"<td style=\"font-weight:bold; padding-right:4px;\">=</td>"
		"<td align=\"right\" style=\"font-weight:bold; padding-right:8px;\">%1</td>"
		"<td style=\"color:%2; font-weight:bold;\">%3</td>"
		"</tr>"
	).arg(qtUtils::toCurrencyFormat(savingsSum, 3), checkColor, checkText);

	QString savingsAfterExplanation =
		"<table cellspacing=\"2\" cellpadding=\"0\">" +
		row(true, cashAfter, "Barvermögen", false, 3) +
		row(false, foreignAfter, "Fremdanteil", false, 3) +
		row(true, debtAllTime, "Schulden", false, 3) +
		row(false, creditAllTime, "Guthaben", true, 3) +
		sumRow +
		"</table>";

	lEarnings->setText(qtUtils::toCurrencyFormat(report.totalEarnings, 3));
	lSpendings->setText(qtUtils::toCurrencyFormat(report.totalSpendings));

	beforeBox->setTitle(formatHeader(report.stateBefore.date));
	lCashBefore->setText(qtUtils::toCurrencyFormat(report.stateBefore.cash));
	lSavingsBefore->setText(qtUtils::toCurrencyFormat(report.stateBefore.savings, 3));
	lForeignBefore->setText(qtUtils::toCurrencyFormat(report.stateBefore.foreignCash, 3));

	lSavingsDifference->setText(qtUtils::toCurrencyFormat(report.savingsDiff, 3));
	lCashDifference->setText(qtUtils::toCurrencyFormat(report.cashDiff));
	lCashDifference->setToolTip(cashDiffExplanation);
	
	afterBox->setTitle(formatHeader(report.stateAfter.date));
	lSavingsAfter->setText(qtUtils::toCurrencyFormat(report.stateAfter.savings, 3));
	lSavingsAfter->setToolTip(savingsAfterExplanation);
	lCashAfter->setText(qtUtils::toCurrencyFormat(report.stateAfter.cash));
	lForeignAfter->setText(qtUtils::toCurrencyFormat(report.stateAfter.foreignCash, 3));

	// enable instant tooltips on hover
	lCashDifference->installEventFilter(new InstantToolTipFilter(lCashDifference));
	lSavingsAfter->installEventFilter(new InstantToolTipFilter(lSavingsAfter));
}

void BalanceTab::apply() {}

QString BalanceTab::formatHeader(const QDate& date) const
{
	return QStringLiteral("Stand %1").arg(date.toString("dd.MM.yyyy"));
}

void BalanceTab::addSettlement()
{
	BalanceTabSettlementDialog::inputs inputs;

	auto* inputDialog = new BalanceTabSettlementDialog(this);
	if (inputDialog->exec() == QDialog::Accepted)
	{
		// inputs given and OK pressed
		inputs = inputDialog->getInputs();
	}
	else { return; } // cancel pressed

	if (inputs.amount < 1e-9)
		return;

	auto returnMsg = balanceService.addShareSettlement(
		request::ShareSettlement{
			.amount = inputs.amount,
			.comment = inputs.comment 
		});

	Q_EMIT instantChangesMade();

	switch (returnMsg) // currently not needed; can be used for error presentation like an error dialog if needed
	{
	case AddSettlementException::none: break;
	case AddSettlementException::amountZero: break;
	case AddSettlementException::amountNegative: break;
	case AddSettlementException::amountGreaterThanTotalForeignShare: break;
	}

	refresh();
}