#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <array>

class CashRegisterSystem : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystem(QMainWindow* parent = nullptr);	
private:
	std::array<QWidget*,3> tabs;
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;

	enum activeTab {activeTabAdd = 0, activeTabPay, activeTabBalance};
	int activeTab = 0;
	std::array<bool, 3> loadedTabs = {false};

	void initUi();
	void initializeTab(QTabWidget* tabs, int tabNum);
};