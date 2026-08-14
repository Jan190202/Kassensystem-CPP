#pragma once
#include "BaseTab.h"
#include <QDate> 

class QTableView;
class QLabel;

class BalanceTab : public BaseTab
{
	Q_OBJECT
public:
	BalanceTab(QWidget* parent = nullptr);
	virtual void initialize() override;
private:
	QTableView* tblSpendings	= nullptr;
	QTableView* tblEarnings		= nullptr;
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

	enum btnIndex 
	{
		addEarning, addSpending
	};

	void addEntry(int index);
};