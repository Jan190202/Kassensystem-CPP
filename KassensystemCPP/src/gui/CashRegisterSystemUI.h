#pragma once
#include <QMainWindow>
#include <array>
#include "BaseTab.h"
class QPushButton;

class CashRegisterSystemUI : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystemUI(QWidget* parent = nullptr);
private:
	std::array<BaseTab*, 3> tabs;
	QPushButton* btnCancel;
	QPushButton* btnApply;
	QPushButton* btnSave;

	enum class tabIndex { pay=0, add, balance };
	int activeTab = 0;
	std::array<bool, 3> loadedTabs = {false};

	void initUi();
	void initializeTab(int tabNum);
};