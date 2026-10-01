#pragma once
#include "BaseTab.h"
#include "app/RepositoryBundle.h"
#include "GuiTypes.h"

class QComboBox;
class QTableView;
class QSqlDatabase;

class ManualTab : public BaseTab
{
	Q_OBJECT
public:
	ManualTab(const LowerButtonBundle& lowerButtons, const RepositoryBundle& repoBundle, QSqlDatabase& db, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	QComboBox* tableSelect;
	QTableView* tableView;

	QSqlDatabase& db;

	const LowerButtonBundle& lowerButtons;
	const RepositoryBundle& repoBundle;
};