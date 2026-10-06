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

class BalanceTab : public BaseTab
{
	Q_OBJECT
public:
	BalanceTab(BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	void addEntry(BtnIndex mode);
	void addSettlement();

	QString formatHeader(const QDate& date) const;
	void refreshTables(const registerFinancials::Report& report) const;
	void refreshLables(const registerFinancials::Report& report) const;

	QTableWidget* tblSpendings = nullptr;
	QTableWidget* tblEarnings = nullptr;
	QLabel* lCashBefore = nullptr;
	QLabel* lForeignBefore = nullptr;
	QLabel* lCashDifference = nullptr;
	TextPopupWidget* popupCashDifference = nullptr;
	QLabel* lCashAfter = nullptr;
	QLabel* lSavingsBefore = nullptr;
	QLabel* lSavingsDifference = nullptr;
	TextPopupWidget* popupSavingsAfter = nullptr;
	QLabel* lSavingsAfter = nullptr;
	QLabel* lForeignAfter = nullptr;
	QLabel* lEarnings = nullptr;
	QLabel* lSpendings = nullptr;
	QGroupBox* beforeBox = nullptr;
	QGroupBox* afterBox = nullptr;
	
	BalanceService& balanceService;
	PersonRepository* personRepo;
	registerFinancials::Report report;
};