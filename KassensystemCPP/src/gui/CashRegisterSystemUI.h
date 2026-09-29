#pragma once
#include "BaseTab.h"
#include "GuiTypes.h"
#include "app/ServiceBundle.h"
#include "app/RepositoryBundle.h"
#include "domain/SessionController.h"
#include <QMainWindow>
#include <array>

class QPushButton;

class CashRegisterSystemUI : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystemUI(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller, QWidget* parent = nullptr);
private:
	enum class TabIndex
	{
		pay = 0, add, balance
	};

	std::array<BaseTab*, 3> tabs;
	LowerButtonBundle lowerButtons;

	int activeTab = 0;
	std::array<bool, 3> loadedTabs = {false};

	void initUi(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller);
	void changeTab(TabIndex idx);
};