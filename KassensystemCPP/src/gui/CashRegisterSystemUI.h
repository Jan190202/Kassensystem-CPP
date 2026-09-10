#pragma once

#include "BaseTab.h"
#include "GuiTypes.h"
#include "app/ServiceBundle.h"
#include "app/RepositoryBundle.h"

#include <QMainWindow>
#include <array>

class QPushButton;

class CashRegisterSystemUI : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystemUI(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, QWidget* parent = nullptr);
private:
	std::array<BaseTab*, 3> tabs;
	LowerButtonBundle lowerButtons;

	enum class TabIndex { Pay=0, Add, Balance };
	int activeTab = 0;
	std::array<bool, 3> loadedTabs = {false};

	void initUi(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle);
	void changeTab(TabIndex idx);
};