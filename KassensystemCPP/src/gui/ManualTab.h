#pragma once
#include "BaseTab.h"
#include "app/RepositoryBundle.h"
#include "GuiTypes.h"

class QComboBox;
class QTableView;
class QSqlDatabase;
class QSqlTableModel;
class QCheckBox;

class ManualTab : public BaseTab
{
	Q_OBJECT
public:
	ManualTab(const LowerButtonBundle& lowerButtons, const RepositoryBundle& repoBundle, QSqlDatabase& db, PendingChangeLog& log, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	QComboBox* tableSelect;
	QCheckBox* toggleNameSelect;
	QComboBox* nameSelect;
	QTableView* tableView;

	QSqlDatabase& db;
	QSqlTableModel* model;

	PendingChangeLog& log;

	const LowerButtonBundle& lowerButtons;
	const RepositoryBundle& repoBundle;
};