#pragma once
#include <QMainWindow>
#include <array>

class CashRegisterSystem : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystem(QMainWindow* parent = nullptr);
private slots:
	
private:
	enum activeTab {activeTabAdd = 0, activeTabPay, activeTabBalance};
	int activeTab = 0;
	int nTabs = 3;
	std::array<bool, 3> loadedTabs = {false};
	void initUi();
	void initializeTab(QTabWidget* tabs, int tabNum);
	//QTabWidget* tabs;
};