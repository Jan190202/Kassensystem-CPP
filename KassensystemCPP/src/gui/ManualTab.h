#pragma once
#include "BaseTab.h"
#include "app/RepositoryBundle.h"
#include "GuiTypes.h"
#include <QStyledItemDelegate>

class QComboBox;
class QTableView;
class QSqlDatabase;
class QSqlTableModel;
class QCheckBox;
class QPushButton;


class ReadOnlyDelegate : public QStyledItemDelegate 
{
public:
	using QStyledItemDelegate::QStyledItemDelegate;

	QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const override 
	{
		return nullptr; // returns no editor -> read-only
	}
};

class ManualTab : public BaseTab
{
	Q_OBJECT
public:
	ManualTab(const LowerButtonBundle& lowerButtons, const RepositoryBundle& repoBundle, QSqlDatabase& db, PendingChangeLog& log, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	void addTableEntry();

	QComboBox* tableSelect;
	QPushButton* btnAddEntry;
	QCheckBox* toggleNameSelect;
	QComboBox* nameSelect;
	QTableView* tableView;

	QSqlDatabase& db;
	QSqlTableModel* model;

	PendingChangeLog& log;

	const LowerButtonBundle& lowerButtons;
	const RepositoryBundle& repoBundle;
};