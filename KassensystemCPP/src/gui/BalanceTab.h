#pragma once
#include "BaseTab.h"
#include "GuiTypes.h"
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
	BalanceTab(const LowerButtonBundle& lowerButtons, BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;
	virtual void save() override;

private:
	void addEntry(BtnIndex mode);
	void addSettlement();

	QString formatHeader(const QDate& date) const;
	void refreshTables() const;
	void refreshLables(const registerFinancials::Report& report) const;

	QTableWidget* tblSpendings = nullptr;
	QTableWidget* tblEarnings = nullptr;
	QLabel* lCashBefore = nullptr;
	QLabel* lForeignBefore = nullptr;
	QLabel* lCashDifference = nullptr;
	QLabel* lCashAfter = nullptr;
	QLabel* lSavingsBefore = nullptr;
	QLabel* lSavingsDifference = nullptr;
	QLabel* lSavingsAfter = nullptr;
	QLabel* lForeignAfter = nullptr;
	QLabel* lEarnings = nullptr;
	QLabel* lSpendings = nullptr;
	QGroupBox* beforeBox = nullptr;
	QGroupBox* afterBox = nullptr;
	
	BalanceService& balanceService;
	PersonRepository* personRepo;
	const LowerButtonBundle& lowerButtons;
};