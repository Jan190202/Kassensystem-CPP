#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <array>
#include "BaseTab.h"

class CashRegisterSystem : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystem(QMainWindow* parent = nullptr);	
private:
	std::array<BaseTab*,3> tabs;
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;

	enum class tabIndex {add = 0, pay, balance};
	int activeTab = 0;
	std::array<bool, 3> loadedTabs = {false};

	void initUi();
	void initializeTab(int tabNum);
};