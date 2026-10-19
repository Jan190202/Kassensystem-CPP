#pragma once
#include "gui/interfaces/BaseTab.h"
#include "gui/types/GuiTypes.h"
#include "app/RepositoryBundle.h"
#include <QStyledItemDelegate>
#include <QDoubleSpinBox>

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

class DoubleDelegate : public QStyledItemDelegate
{
public:
	using QStyledItemDelegate::QStyledItemDelegate;

	QString displayText(const QVariant& v, const QLocale& l) const override
	{
		if (v.typeId() == QMetaType::Double)
			return l.toString(v.toDouble(), 'f', QLocale::FloatingPointShortest);
		return QStyledItemDelegate::displayText(v, l);
	}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& idx) const override
	{
		QWidget* w = QStyledItemDelegate::createEditor(parent, opt, idx);
		if (auto* sb = qobject_cast<QDoubleSpinBox*>(w))
			sb->setDecimals(8);
		return w;
	}
};

class ManualTab : public BaseTab
{
	Q_OBJECT
public:
	ManualTab(const RepositoryBundle& repoBundle, QSqlDatabase& db, PendingChangeLog& log, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	void nameChanged();
	void displayTable();
	void addTableEntry();
	void deleteTableEntry();
	void unhideAllRows();

	QComboBox* tableSelect;
	QPushButton* btnAddEntry;
	QPushButton* btnDeleteEntry;
	QCheckBox* toggleNameSelect;
	QComboBox* nameSelect;
	QTableView* tableView;

	QSqlDatabase& db;
	QSqlTableModel* model;

	PendingChangeLog& log;

	const RepositoryBundle& repoBundle;
};