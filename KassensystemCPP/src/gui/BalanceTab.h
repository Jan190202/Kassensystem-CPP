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
	BalanceTab(const LowerButtonBundle& lowerButtons, BalanceService& balanceService, PersonRepository* personRepo, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
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
	PersonRepository* personRepo;
	const LowerButtonBundle& lowerButtons;
private slots:
	void collectResults();
};