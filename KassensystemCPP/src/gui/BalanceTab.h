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
	QTableWidget* tblSpendings	= nullptr;
	QTableWidget* tblEarnings	= nullptr;
	QLabel* lCashBefore			= nullptr;
	QLabel* lForeignBefore		= nullptr;
	QLabel* lCashDifference		= nullptr;
	QLabel* lCashAfter			= nullptr;
	QLabel* lSavingsBefore		= nullptr;
	QLabel* lSavingsDifference	= nullptr;
	QLabel* lSavingsAfter		= nullptr;
	QLabel* lForeignAfter		= nullptr;
	QLabel* lEarnings			= nullptr;
	QLabel* lSpendings			= nullptr;
	QGroupBox* beforeBox		= nullptr;
	QGroupBox* afterBox			= nullptr;

	void addEntry(BtnIndex mode);
	QString formatHeader(const QDate& date) const;
	void refreshTables(registerFinancials::Report) const;
	void refreshLables(registerFinancials::Report) const;
	void addSettlement();

	BalanceService& balanceService;
	PersonRepository* personRepo;
	const LowerButtonBundle& lowerButtons;
};