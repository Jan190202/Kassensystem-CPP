#pragma once
#include "gui/interfaces/BaseTab.h"
#include "gui/types/GuiTypes.h"
#include "gui/components/TextPopupWidget.h"
#include "domain/services/BalanceService.h"
#include "common/Utils.h"
#include <QDate>

class QTableWidget;
class QLabel;
class QGroupBox;
class QPushButton;

class BalanceTab : public BaseTab
{
	Q_OBJECT
public:
	BalanceTab(BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	// actions
	void addEntry(BtnIndex mode);
	void addSettlement();
	void startReview();
	void showPreviousPeriod();
	void showNextPeriod();

	// view updates
	void showEmptyState();
	void refreshNavigation();
	void refreshTables();
	void refreshConsumption();
	void refreshLabels();
	void setPeriodActionsEnabled(bool enabled);

	QString formatStartHeader() const;
	QString formatEndHeader() const;
	QString formatPeriod() const;

	// navigation
	QPushButton* btnPrevPeriod = nullptr;
	QPushButton* btnNextPeriod = nullptr;
	QLabel* lPeriod = nullptr;
	QLabel* lPeriodWarning = nullptr;

	// journal
	QTableWidget* tblSpendings = nullptr;
	QTableWidget* tblEarnings = nullptr;
	QLabel* lEarnings = nullptr;
	QLabel* lSpendings = nullptr;
	QPushButton* btnAddEarning = nullptr;
	QPushButton* btnAddSpending = nullptr;

	// consumption
	QGroupBox* consumptionBox = nullptr;
	QTableWidget* tblConsumption = nullptr;
	QLabel* lConsumptionSummary = nullptr;

	// start / change / end
	QGroupBox* beforeBox = nullptr;
	QGroupBox* afterBox = nullptr;
	QLabel* lCashBefore = nullptr;
	QLabel* lSavingsBefore = nullptr;
	QLabel* lForeignBefore = nullptr;
	QLabel* lCashDifference = nullptr;
	QLabel* lSavingsDifference = nullptr;
	QLabel* lCashAfter = nullptr;
	QLabel* lSavingsAfter = nullptr;
	QLabel* lForeignAfter = nullptr;
	TextPopupWidget* popupCashDifference = nullptr;
	TextPopupWidget* popupSavingsAfter = nullptr;
	QPushButton* btnSettleForeign = nullptr;
	QPushButton* btnReview = nullptr;

	BalanceService& balanceService;
	PersonRepository* personRepo;

	// -1 = newest period (last review -> now)
	int periodIndex = -1;
	size_t periodCount = 0;
	registerFinancials::PeriodReport report;
};