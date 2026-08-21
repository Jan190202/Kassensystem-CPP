#pragma once
#include <QMainWindow>
#include <array>
#include "BaseTab.h"
#include "GuiTypes.h"

class QPushButton;

class CashRegisterSystemUI : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystemUI(const ServiceBundle& services, QWidget* parent = nullptr);
private:
	std::array<BaseTab*, 3> tabs;
	LowerButtonBundle lowerButtons;

	enum class tabIndex { pay=0, add, balance };
	int activeTab = 0;
	std::array<bool, 3> loadedTabs = {false};

	void initUi(const ServiceBundle& services);
	void initializeTab(int tabNum);
};