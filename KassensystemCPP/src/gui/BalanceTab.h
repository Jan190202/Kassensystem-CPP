#pragma once
#include "BaseTab.h"
#include "GuiTypes.h"
#include "domain/services/BalanceService.h"

#include <QDate> 

class QTableWidget;
class QLabel;

class BalanceTab : public BaseTab
{
	Q_OBJECT
public:
	BalanceTab(const LowerButtonBundle& lowerButtons, BalanceService& balanceService, QWidget* parent = nullptr);
	virtual void initialize() override;
private:
	QTableWidget* tblSpendings	= nullptr;
	QTableWidget* tblEarnings	= nullptr;
	QLabel* lCashBefore			= nullptr;
	QLabel* lCashDifference		= nullptr;
	QLabel* lCashAfter			= nullptr;
	QLabel* lSavingsBefore		= nullptr;
	QLabel* lSavingsDifference	= nullptr;
	QLabel* lSavingsAfter		= nullptr;
	QLabel* lEarnings			= nullptr;
	QLabel* lSpendings			= nullptr;

	QDate dateBefore;
	QDate dateAfter				= QDate::currentDate();

	void addEntry(btnIndex mode);

	BalanceService& balanceService;
	const LowerButtonBundle& lowerButtons;
private Q_SLOTS:
	void collectResults();
};