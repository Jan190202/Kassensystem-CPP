#pragma once
#include "BaseTab.h"
#include "commons/CommonTypes.h"

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

	void addEntry(btnIndex mode);
private slots:
	void collectResults();
};