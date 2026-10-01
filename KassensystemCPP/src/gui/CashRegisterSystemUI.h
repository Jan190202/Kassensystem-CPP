#pragma once
#include "BaseTab.h"
#include "GuiTypes.h"
#include "app/ServiceBundle.h"
#include "app/RepositoryBundle.h"
#include "domain/SessionController.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <array>

class QPushButton;

class CashRegisterSystemUI : public QMainWindow
{
	Q_OBJECT
public:
	CashRegisterSystemUI(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller, QSqlDatabase& db, QWidget* parent = nullptr);
private:
	enum class TabIndex
	{
		pay = 0, add, balance, manual
	};

	std::array<BaseTab*, 4> tabs;
	LowerButtonBundle lowerButtons;

	int activeTab = 0;
	std::array<bool, 4> loadedTabs = {false};

	void initUi(const ServiceBundle& serviceBundle, const RepositoryBundle& repoBundle, const SessionController& controller, QSqlDatabase& db);
	void changeTab(TabIndex idx);
};