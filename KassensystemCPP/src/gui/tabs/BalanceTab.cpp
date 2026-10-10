#include "gui/tabs/BalanceTab.h"
#include "gui/dialogs/BalanceTabAddEntryDialog.h"
#include "gui/dialogs/BalanceTabSettlementDialog.h"
#include "gui/dialogs/BalanceTabReviewDialog.h"
#include "gui/types/GuiTypes.h"
#include "gui/IconLoader.h"
#include "qtutils/QtConversions.h"
#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <string>
#include <array>
#include <algorithm>
#include <cmath>

namespace
{
	constexpr int rowHeight = 30;
	constexpr double tolerance = 5e-4; // half of the display resolution (3 decimals)

	QString currency(double value, int decimals = 2)
	{
		return qtUtils::toCurrencyFormat(value, decimals);
	}

	// one line of a popup table: "+ 12.00 € description"
	QString popupRow(bool addsPositively, double num, const QString& desc, int decimals = 2)
	{
		const QString color = addsPositively ? "#2e8b57" : "#c0392b"; // green / red
		const QString sign = addsPositively ? "+" : "-";
		const QString val = num >= 0 ? currency(num, decimals) : ("(" + currency(num, decimals) + ")");

		return QString(
			"<tr>"
			"<td style=\"color:%1; font-weight:bold; padding-right:4px;\">%2</td>"
			"<td align=\"right\" style=\"font-weight:bold; padding-right:8px;\">%3</td>"
			"<td>%4</td>"
			"</tr>"
		).arg(color, sign, val, desc);
	}

	// sum line with consistency indicator
	QString popupSumRow(double sum, bool consistent, int decimals = 2)
	{
		const QString color = consistent ? "#2e8b57" : "#c0392b";
		const QString text = consistent ? "&#10004; korrekt" : "&#10008; inkorrekt";

		return QString(
			"<tr><td colspan=\"3\"><hr></td></tr>"
			"<tr>"
			"<td style=\"font-weight:bold; padding-right:4px;\">=</td>"
			"<td align=\"right\" style=\"font-weight:bold; padding-right:8px;\">%1</td>"
			"<td style=\"color:%2; font-weight:bold;\">%3</td>"
			"</tr>"
		).arg(currency(sum, decimals), color, text);
	}

	void configureAmount(QLabel* label)
	{
		label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		label->setMinimumWidth(90);
		label->setMinimumHeight(rowHeight);
	}

	QTableWidgetItem* amountItem(double value, int decimals = 3)
	{
		auto* item = new QTableWidgetItem(currency(value, decimals));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		return item;
	}
}

BalanceTab::BalanceTab(BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent)
	: balanceService(balanceService), personRepo(personRepo), BaseTab(parent) {}

void BalanceTab::initialize()
{
	// buttons
	btnAddEarning = new QPushButton(QStringLiteral("+ Einnahme"), this);
	btnAddSpending = new QPushButton(QStringLiteral("+ Ausgabe"), this);

	btnSettleForeign = new QPushButton(this);
	btnSettleForeign->setIcon(iconLoader::getIcon("refresh-arrow.png"));
	btnSettleForeign->setToolTip(QStringLiteral("Fremdanteil abrechnen"));

	btnReview = new QPushButton(this);
	btnReview->setIcon(iconLoader::getIcon("calculator.png"));
	btnReview->setToolTip(QStringLiteral("Kassensturz durchführen"));

	btnPrevPeriod = new QPushButton(QStringLiteral("\u25C0"), this);
	btnPrevPeriod->setToolTip(QStringLiteral("Vorheriger Zeitraum"));
	btnNextPeriod = new QPushButton(QStringLiteral("\u25B6"), this);
	btnNextPeriod->setToolTip(QStringLiteral("Nächster Zeitraum"));

	btnSettleForeign->setFixedSize(rowHeight, rowHeight);
	btnReview->setFixedSize(rowHeight, rowHeight);
	btnPrevPeriod->setFixedSize(rowHeight + 10, rowHeight);
	btnNextPeriod->setFixedSize(rowHeight + 10, rowHeight);

	// tables
	tblEarnings = new QTableWidget(this);
	tblSpendings = new QTableWidget(this);
	tblEarnings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tblSpendings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	tblConsumption = new QTableWidget(3, 5, this);
	tblConsumption->setHorizontalHeaderLabels(
		{ QString(), QStringLiteral("Beginn"), QStringLiteral("+ Zugang"), QStringLiteral("- Abgang"), QStringLiteral("Ende") });
	tblConsumption->verticalHeader()->hide();
	tblConsumption->verticalHeader()->setDefaultSectionSize(rowHeight);
	tblConsumption->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tblConsumption->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tblConsumption->setSelectionMode(QAbstractItemView::NoSelection);
	tblConsumption->setFocusPolicy(Qt::NoFocus);
	tblConsumption->setFixedHeight(tblConsumption->horizontalHeader()->sizeHint().height() + 3 * rowHeight + 4);

	// labels
	lPeriod = new QLabel(this);
	lPeriod->setAlignment(Qt::AlignCenter);
	QFont periodFont = lPeriod->font();
	periodFont.setBold(true);
	lPeriod->setFont(periodFont);

	lPeriodWarning = new QLabel(this);
	lPeriodWarning->setAlignment(Qt::AlignCenter);
	lPeriodWarning->setStyleSheet("color:#d68910;");
	lPeriodWarning->hide();

	lConsumptionSummary = new QLabel(this);
	lConsumptionSummary->setTextFormat(Qt::RichText);
	lConsumptionSummary->setWordWrap(true);

	lCashBefore = new QLabel(currency(0.0), this);
	lCashDifference = new QLabel(currency(0.0), this);
	lCashAfter = new QLabel(currency(0.0), this);
	lSavingsBefore = new QLabel(currency(0.0, 3), this);
	lSavingsDifference = new QLabel(currency(0.0, 3), this);
	lSavingsAfter = new QLabel(currency(0.0, 3), this);
	lForeignBefore = new QLabel(currency(0.0, 3), this);
	lForeignAfter = new QLabel(currency(0.0, 3), this);
	lEarnings = new QLabel(currency(0.0), this);
	lSpendings = new QLabel(currency(0.0), this);

	popupCashDifference = new TextPopupWidget(TextPopupWidget::PopupPos::bottomRight, this);
	popupSavingsAfter = new TextPopupWidget(TextPopupWidget::PopupPos::bottomRight, this);
	popupCashDifference->setFixedHeight(rowHeight);
	popupSavingsAfter->setFixedHeight(rowHeight);

	for (QLabel* label : { lCashBefore, lCashDifference, lCashAfter, lSavingsBefore, lSavingsDifference,
						   lSavingsAfter, lForeignBefore, lForeignAfter, lEarnings, lSpendings })
		configureAmount(label);

	// navigation row
	auto* navigationLayout = new QHBoxLayout();
	navigationLayout->setContentsMargins(0, 0, 0, 0);
	navigationLayout->setSpacing(10);
	navigationLayout->addWidget(btnPrevPeriod);
	navigationLayout->addWidget(lPeriod, 1);
	navigationLayout->addWidget(btnNextPeriod);

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

	// consumption
	consumptionBox = new QGroupBox(QStringLiteral("Getränke"), this);
	auto* consumptionLayout = new QVBoxLayout(consumptionBox);
	consumptionLayout->setContentsMargins(12, 16, 12, 12);
	consumptionLayout->setSpacing(8);
	consumptionLayout->addWidget(tblConsumption);
	consumptionLayout->addWidget(lConsumptionSummary);

	// start
	beforeBox = new QGroupBox(this);
	auto* beforeLayout = new QFormLayout(beforeBox);
	beforeLayout->setContentsMargins(12, 16, 12, 12);
	beforeLayout->setHorizontalSpacing(16);
	beforeLayout->setVerticalSpacing(7);
	beforeLayout->addRow(QStringLiteral("Bestand:"), lSavingsBefore);
	beforeLayout->addRow(QStringLiteral("Bar:"), lCashBefore);
	beforeLayout->addRow(QStringLiteral("davon Fremdanteil:"), lForeignBefore);

	// change
	auto* differenceBox = new QGroupBox(QStringLiteral("Veränderung"), this);
	auto* differenceLayout = new QFormLayout(differenceBox);
	differenceLayout->setContentsMargins(12, 16, 12, 12);
	differenceLayout->setHorizontalSpacing(16);
	differenceLayout->setVerticalSpacing(7);
	differenceLayout->addRow(QStringLiteral("Bestand:"), lSavingsDifference);

	auto* cashDifferenceLayout = new QHBoxLayout();
	cashDifferenceLayout->setContentsMargins(0, 0, 0, 0);
	cashDifferenceLayout->setSpacing(8);
	cashDifferenceLayout->addWidget(lCashDifference);
	cashDifferenceLayout->addWidget(popupCashDifference);
	differenceLayout->addRow(QStringLiteral("Bar:"), cashDifferenceLayout);

	// end
	afterBox = new QGroupBox(this);
	auto* afterLayout = new QFormLayout(afterBox);
	afterLayout->setContentsMargins(12, 16, 12, 12);
	afterLayout->setHorizontalSpacing(16);
	afterLayout->setVerticalSpacing(7);

	auto* savingsAfterLayout = new QHBoxLayout();
	savingsAfterLayout->setContentsMargins(0, 0, 0, 0);
	savingsAfterLayout->setSpacing(8);
	savingsAfterLayout->addWidget(lSavingsAfter);
	savingsAfterLayout->addWidget(popupSavingsAfter);
	afterLayout->addRow(QStringLiteral("Bestand:"), savingsAfterLayout);

	auto* cashAfterLayout = new QHBoxLayout();
	cashAfterLayout->setContentsMargins(0, 0, 0, 0);
	cashAfterLayout->setSpacing(8);
	cashAfterLayout->addWidget(lCashAfter);
	cashAfterLayout->addWidget(btnReview);
	afterLayout->addRow(QStringLiteral("Bar:"), cashAfterLayout);

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
	mainLayout->addLayout(navigationLayout);
	mainLayout->addWidget(lPeriodWarning);
	mainLayout->addLayout(tableLayout, 1);
	mainLayout->addWidget(consumptionBox);
	mainLayout->addLayout(summaryLayout);

	refresh();

	connect(btnAddEarning, &QPushButton::clicked, this, [this]() { addEntry(BtnIndex::addEarning); });
	connect(btnAddSpending, &QPushButton::clicked, this, [this]() { addEntry(BtnIndex::addSpending); });
	connect(btnSettleForeign, &QPushButton::clicked, this, [this]() { addSettlement(); });
	connect(btnReview, &QPushButton::clicked, this, [this]() { startReview(); });
	connect(btnPrevPeriod, &QPushButton::clicked, this, [this]() { showPreviousPeriod(); });
	connect(btnNextPeriod, &QPushButton::clicked, this, [this]() { showNextPeriod(); });
}

// actions

void BalanceTab::addEntry(BtnIndex mode)
{
	BalanceTabAddEntryDialog::inputs inputs;

	std::vector<entry::Person> personVec = personRepo->getAllPersonEntries();
	auto* inputDialog = new BalanceTabAddEntryDialog(mode, personVec, this);
	if (inputDialog->exec() == QDialog::Accepted)
	{
		inputs = inputDialog->getInputs();
	}
	else { return; } // cancel pressed

	balanceService.addBalanceItem(
		request::Balance{
			.type = mode == BtnIndex::addEarning ? BalanceType::earning : BalanceType::spending,
			.description = inputs.description,
			.amount = inputs.amount,
			.dateBooked = inputs.date,
			.comment = inputs.comment,
			.coveringPerson = inputs.coveringPerson
		});

	Q_EMIT instantChangesMade();

	refresh();
}

void BalanceTab::addSettlement()
{
	BalanceTabSettlementDialog::inputs inputs;

	auto* inputDialog = new BalanceTabSettlementDialog(this);
	if (inputDialog->exec() == QDialog::Accepted)
	{
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

void BalanceTab::startReview()
{
	const double expectedCash = periodCount == 0 ? 0.0 : report.end.cash;

	BalanceTabReviewDialog dialog(expectedCash, this);
	if (dialog.exec() != QDialog::Accepted)
		return;

	const auto inputs = dialog.getInputs();

	// TBD
	//balanceService.addFinancialReview(
	//	request::FinancialReview{
	//		.countedCash = inputs.countedCash,
	//		.expectedCash = expectedCash,
	//		.comment = inputs.comment
	//	});

	Q_EMIT instantChangesMade();

	periodIndex = -1; 
	refresh();
}

void BalanceTab::showPreviousPeriod()
{
	if (periodIndex <= 0)
		return;

	--periodIndex;
	refresh();
}

void BalanceTab::showNextPeriod()
{
	if (periodIndex < 0 || static_cast<size_t>(periodIndex) + 1 >= periodCount)
		return;

	++periodIndex;
	refresh();
}

// refresh

void BalanceTab::refresh()
{
	periodCount = 0; // balanceService.getPeriodCount(); // TBD

	if (periodCount == 0)
	{
		showEmptyState();
		return;
	}

	if (periodIndex < 0 || static_cast<size_t>(periodIndex) >= periodCount)
		periodIndex = static_cast<int>(periodCount) - 1;

	report = registerFinancials::PeriodReport{}; // balanceService.getPeriodReport(static_cast<size_t>(periodIndex)); // TBD

	refreshNavigation();
	refreshTables();
	refreshConsumption();
	refreshLabels();
}

void BalanceTab::showEmptyState()
{
	tblEarnings->setRowCount(0);
	tblSpendings->setRowCount(0);
	tblConsumption->clearContents();

	lPeriod->setText(QStringLiteral("Noch kein Kassensturz vorhanden - bitte zuerst den Anfangsbestand erfassen"));
	lPeriodWarning->hide();
	lConsumptionSummary->clear();
	popupCashDifference->setRichText(QString());
	popupSavingsAfter->setRichText(QString());

	for (QLabel* label : { lCashBefore, lCashDifference, lCashAfter, lSavingsBefore, lSavingsDifference,
						   lSavingsAfter, lForeignBefore, lForeignAfter, lEarnings, lSpendings })
		label->setText(currency(0.0));

	beforeBox->setTitle(QStringLiteral("Beginn"));
	afterBox->setTitle(QStringLiteral("Ende"));

	btnPrevPeriod->setEnabled(false);
	btnNextPeriod->setEnabled(false);
	setPeriodActionsEnabled(false);
	btnReview->setEnabled(true); 
}

void BalanceTab::refreshNavigation()
{
	lPeriod->setText(formatPeriod());

	btnPrevPeriod->setEnabled(periodIndex > 0);
	btnNextPeriod->setEnabled(static_cast<size_t>(periodIndex) + 1 < periodCount);

	setPeriodActionsEnabled(report.isCurrent);

	lPeriodWarning->setVisible(report.changedSinceReview);
	if (report.changedSinceReview)
		lPeriodWarning->setText(QStringLiteral("&#9888; Seit dem Kassensturz wurden Einträge in diesem Zeitraum verändert "
			"(erwartete Kasse weicht vom damals gespeicherten Wert ab)"));
}

void BalanceTab::setPeriodActionsEnabled(bool enabled)
{
	btnAddEarning->setEnabled(enabled);
	btnAddSpending->setEnabled(enabled);
	btnSettleForeign->setEnabled(enabled);
	btnReview->setEnabled(enabled);
}

void BalanceTab::refreshTables()
{
	using TableAllocation = std::pair<QTableWidget*, BalanceType>;
	const std::array<TableAllocation, 2> allocations{ {
		{ tblEarnings,  BalanceType::earning },	 // only real journal entries; consumption has its own section
		{ tblSpendings, BalanceType::spending }
	} };

	for (const auto& [table, type] : allocations)
	{
		table->clearContents();

		// entries booked after the start review, up to and including the end review (or open end for the current period)
		std::vector<entry::Balance> bEntries = {}; // balanceService.getBalanceEntries(type, report.periodStart, report.periodEnd); // TBD

		std::sort(bEntries.begin(), bEntries.end(), [](const entry::Balance& a, const entry::Balance& b)
			{
				return a.dateBooked > b.dateBooked;
			});

		table->setRowCount(static_cast<int>(bEntries.size()));
		table->setColumnCount(3); // description, amount, dateBooked
		table->setHorizontalHeaderLabels(qtUtils::strVecToQStrList({ "Beschreibung", "Betrag (" + utils::eurSymbol() + ")", "Datum" }));

		for (size_t row = 0; row < bEntries.size(); row++)
		{
			const entry::Balance& bEntry = bEntries.at(row);

			table->setItem(static_cast<int>(row), 0, new QTableWidgetItem(QString::fromStdString(bEntry.description)));
			table->setItem(static_cast<int>(row), 1, new QTableWidgetItem(QString::number(bEntry.amount, 'f', 2)));
			table->setItem(static_cast<int>(row), 2, new QTableWidgetItem(bEntry.dateBooked.toQString()));
		}

		table->resizeColumnsToContents();
	}
}

void BalanceTab::refreshConsumption()
{
	const auto& c = report.consumption;

	struct Line
	{
		QString name;
		registerFinancials::RollForwardRow row;
	};

	const std::array<Line, 3> lines
	{ 
		{
			{ QStringLiteral("Offene Verbräuche (Schulden)"),	c.debt },
			{ QStringLiteral("Fremdanteil"),					c.foreignShare },
			{ QStringLiteral("Guthaben"),						c.credit }
		} 
	};

	for (int i = 0; i < static_cast<int>(lines.size()); i++)
	{
		const auto& r = lines.at(i).row;

		tblConsumption->setItem(i, 0, new QTableWidgetItem(lines.at(i).name));
		tblConsumption->setItem(i, 1, amountItem(r.begin));
		tblConsumption->setItem(i, 2, amountItem(r.added));
		tblConsumption->setItem(i, 3, amountItem(r.removed));

		// end value is read directly from the database, the roll-forward must reproduce it
		auto* endItem = amountItem(r.endDirect);
		const double rolledForward = r.begin + r.added - r.removed;
		if (std::abs(rolledForward - r.endDirect) > tolerance)
		{
			endItem->setForeground(QColor("#c0392b"));
			endItem->setToolTip(QStringLiteral("Fortschreibung (Beginn + Zugang - Abgang) ergibt %1").arg(currency(rolledForward, 3)));
		}
		tblConsumption->setItem(i, 4, endItem);
	}

	// bridge: how journal + consumption share + review difference add up to the change of the savings
	const double savingsChange = report.end.savings - report.start.savings;
	const double bridge = report.details.departmentEarnings + c.departmentShare
		- report.details.departmentSpendings + report.details.cashCorrection;
	const bool bridgeOk = std::abs(bridge - savingsChange) < tolerance;

	QString bridgeText = QStringLiteral("Einnahmen %1 + Abteilungsanteil Getränke %2 - Ausgaben %3")
		.arg(currency(report.details.departmentEarnings), currency(c.departmentShare), currency(report.details.departmentSpendings));
	if (std::abs(report.details.cashCorrection) > 1e-9)
		bridgeText += QStringLiteral(" %1 Kassendifferenz %2")
		.arg(report.details.cashCorrection >= 0 ? "+" : "-", currency(std::abs(report.details.cashCorrection)));
	bridgeText += QStringLiteral(" = <b>Veränderung Bestand %1</b> <span style=\"color:%2; font-weight:bold;\">%3</span>")
		.arg(currency(bridge, 3), bridgeOk ? "#2e8b57" : "#c0392b", bridgeOk ? "&#10004;" : "&#10008;");

	lConsumptionSummary->setText(
		QStringLiteral("Verbrauch gesamt: %1 &middot; davon Abteilungsanteil: %2 &middot; davon Fremdanteil: %3<br>%4")
		.arg(currency(c.totalConsumption), currency(c.departmentShare, 3), currency(c.foreignShare.added, 3), bridgeText));
}

void BalanceTab::refreshLabels()
{
	const auto& d = report.details;

	// popup: how the cash changed in this period
	const double cashChange = report.end.cash - report.start.cash;
	const double cashFlowSum = d.departmentEarnings - d.departmentSpendings + d.paidDebt - d.settledValue
		+ d.depositedCredit + d.cashCorrection;
	const bool cashFlowOk = std::abs(cashFlowSum - cashChange) < tolerance;

	QString cashDiffExplanation =
		"<table cellspacing=\"2\" cellpadding=\"0\">" +
		popupRow(true, d.departmentEarnings, "Einnahmen") +
		popupRow(false, d.departmentSpendings, "Ausgaben") +
		popupRow(true, d.paidDebt, "bezahlte Verbräuche") +
		popupRow(false, d.settledValue, "Abgabe an Verein") +
		popupRow(true, d.depositedCredit, "Guthaben");
	if (!report.isCurrent && std::abs(d.cashCorrection) > 1e-9)
		cashDiffExplanation += popupRow(d.cashCorrection >= 0, std::abs(d.cashCorrection), "Kassendifferenz (Kassensturz)");
	cashDiffExplanation += popupSumRow(cashFlowSum, cashFlowOk) + "</table>";

	// popup: what the savings at the end consist of
	const double compositionSum = report.end.cash - report.end.foreignCash + report.end.debt - report.end.credit;
	const double bridge = d.departmentEarnings + report.consumption.departmentShare
		- d.departmentSpendings + d.cashCorrection;
	const bool savingsMatch = std::abs(compositionSum - (report.start.savings + bridge)) < tolerance;

	const QString savingsAfterExplanation =
		"<table cellspacing=\"2\" cellpadding=\"0\">" +
		popupRow(true, report.end.cash, "Barvermögen", 3) +
		popupRow(false, report.end.foreignCash, "Fremdanteil", 3) +
		popupRow(true, report.end.debt, "Schulden", 3) +
		popupRow(false, report.end.credit, "Guthaben", 3) +
		popupSumRow(compositionSum, savingsMatch, 3) +
		"</table>";

	// journal totals (journal entries only)
	lEarnings->setText(currency(report.totalEarnings));
	lSpendings->setText(currency(report.totalSpendings));

	// start
	beforeBox->setTitle(formatStartHeader());
	lCashBefore->setText(currency(report.start.cash));
	lSavingsBefore->setText(currency(report.start.savings, 3));
	lForeignBefore->setText(currency(report.start.foreignCash, 3));

	// change
	lSavingsDifference->setText(currency(report.end.savings - report.start.savings, 3));
	lCashDifference->setText(currency(cashChange));
	popupCashDifference->setRichText(cashDiffExplanation);

	// end
	afterBox->setTitle(formatEndHeader());
	lSavingsAfter->setText(currency(report.end.savings, 3));
	popupSavingsAfter->setRichText(savingsAfterExplanation);
	lCashAfter->setText(currency(report.end.cash));
	lForeignAfter->setText(currency(report.end.foreignCash, 3));
}

void BalanceTab::apply() {}

// formatting

QString BalanceTab::formatStartHeader() const
{
	return QStringLiteral("Kassensturz %1").arg(report.start.date.toString("dd.MM.yyyy"));
}

QString BalanceTab::formatEndHeader() const
{
	return report.isCurrent
		? QStringLiteral("Stand %1 (aktuell)").arg(report.end.date.toString("dd.MM.yyyy"))
		: QStringLiteral("Kassensturz %1").arg(report.end.date.toString("dd.MM.yyyy"));
}

QString BalanceTab::formatPeriod() const
{
	const QString from = report.start.date.toString("dd.MM.yyyy");
	const QString to = report.isCurrent ? QStringLiteral("heute") : report.end.date.toString("dd.MM.yyyy");
	return QStringLiteral("Zeitraum: %1 - %2").arg(from, to);
}