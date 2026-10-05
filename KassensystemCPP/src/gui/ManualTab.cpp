#pragma once
#include "ManualTab.h"
#include "qtutils/QtConversions.h"
#include <QComboBox>
#include <QCheckBox>
#include <QTableView>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QCompleter>
#include <QPushButton>
#include <QMessageBox>
#include <optional>
#include <algorithm>

ManualTab::ManualTab(const RepositoryBundle& repoBundle, QSqlDatabase& db, PendingChangeLog& log, QWidget* parent)
	: repoBundle(repoBundle), db(db), log(log), BaseTab(parent) {}

void ManualTab::initialize()
{
	tableSelect = new QComboBox();
	QStringList tableNames = db.tables();
	std::sort(tableNames.begin(), tableNames.end(), [](const QString& a, const QString& b)
		{
			return a < b;
		});
	tableSelect->insertItems(0, tableNames);
	tableSelect->setEditable(false);
	tableSelect->setMinimumWidth(300);

	btnAddEntry = new QPushButton("+");
	btnAddEntry->setMaximumWidth(30);

	btnDeleteEntry = new QPushButton("-");
	btnDeleteEntry->setMaximumWidth(30);

	nameSelect = new QComboBox(this);
	nameSelect->setEditable(true);
	nameSelect->setCurrentIndex(-1);
	auto* completer = new QCompleter(nameSelect->model(), nameSelect);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setFilterMode(Qt::MatchContains);
	nameSelect->setCompleter(completer);
	nameSelect->setEnabled(false);
	nameSelect->setMinimumWidth(300);

	toggleNameSelect = new QCheckBox("nach Person filtern");
	toggleNameSelect->setChecked(false);

	auto* filterLayout = new QHBoxLayout();
	filterLayout->addWidget(tableSelect);
	filterLayout->addWidget(btnAddEntry);
	filterLayout->addWidget(btnDeleteEntry);
	filterLayout->addStretch();
	filterLayout->addWidget(toggleNameSelect);
	filterLayout->addWidget(nameSelect);

	model = new QSqlTableModel(this);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	tableView = new QTableView();
	tableView->verticalHeader()->hide();
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(filterLayout);
	mainLayout->addWidget(tableView);

	connect(tableSelect, &QComboBox::currentIndexChanged, this, [&]() 
		{
			model->setTable(tableSelect->currentText());
			nameChanged();
			displayTable();
		});

	connect(toggleNameSelect, &QCheckBox::checkStateChanged, this, [&](Qt::CheckState state)
		{
			nameSelect->setEnabled(state == Qt::Checked);
			nameChanged();
		});

	connect(nameSelect, &QComboBox::currentIndexChanged, this, [&]() {nameChanged(); });

	auto flagTemporary = [&]() { Q_EMIT temporaryChangesExist(true); };
	connect(model, &QAbstractItemModel::dataChanged,		this, flagTemporary);
	connect(model, &QAbstractItemModel::headerDataChanged,	this, flagTemporary);
	connect(model, &QAbstractItemModel::rowsInserted,		this, flagTemporary);
	connect(model, &QAbstractItemModel::rowsRemoved,		this, flagTemporary);

	connect(model, &QSqlTableModel::beforeInsert, this, [&](QSqlRecord& record) // TBD: implement row insertion
		{
			log.record("Eintrag in " + model->tableName().toStdString() + " manuell hinzugefügt");
		});

	connect(model, &QSqlTableModel::beforeDelete, this, [&](int row) // TBD: implement row deletion
		{
			log.record("Eintrag in " + model->tableName().toStdString() + " manuell gelöscht"); 
		});

	connect(model, &QSqlTableModel::beforeUpdate, this, [&](int row, QSqlRecord& record)
		{
			log.record("Eintrag in " + model->tableName().toStdString() + " manuell bearbeitet"); 
		});

	connect(btnAddEntry, &QPushButton::clicked, this, [&]() { addTableEntry(); });

	connect(btnDeleteEntry, &QPushButton::clicked, this, [&]() { deleteTableEntry(); });

	refresh();
}

void ManualTab::refresh()
{
	// refresh name select
	QSignalBlocker blocker(nameSelect);

	bool isPlaceholder = false;
	if (nameSelect->currentIndex() == -1) isPlaceholder = true;

	bool isOldIDValid = nameSelect->currentData().canConvert<entry::Person>();
	int64_t oldID = nameSelect->currentData().value<entry::Person>().personEntryID;

	nameSelect->clear();

	// get all person entries and add them in alphabetical order
	std::vector<entry::Person> personVec = repoBundle.personRepo->getAllPersonEntries();
	std::sort(personVec.begin(), personVec.end(), [](const entry::Person& a, const entry::Person& b)
		{
			return a.getFullSpecifier() > b.getFullSpecifier();
		});
	QList<QString> nameList = qtUtils::personVecToQStrList(personVec, &entry::Person::getFullSpecifier);

	std::optional<size_t> indexForOldID;
	for (size_t i = personVec.size(); i-- > 0; ) // loop backward to insert in inverse-alphabetical order (i = size()-1 ... 0)
	{
		nameSelect->addItem(nameList.at(i), QVariant::fromValue(personVec.at(i)));
		if (personVec.at(i).personEntryID == oldID && isOldIDValid) indexForOldID = personVec.size() - i - 1; // == 0 ... size()-1
	}

	if (isPlaceholder) // set to placeholder again
	{
		nameSelect->setCurrentIndex(-1);
	}
	else // set to last selected person, if found
	{
		if (indexForOldID.has_value()) nameSelect->setCurrentIndex(indexForOldID.value());
	}

	// refresh table
	model->setTable(tableSelect->currentText());

	// apply filters and display
	nameChanged();
	displayTable();
}

void ManualTab::nameChanged()
{
	// reset filter
	model->setFilter("");
	
	// search for personID column
	std::optional<QString> personColumnName;
	std::optional<QString> debtColumnName;
	QSqlRecord record = model->record();
	for (int i = 0; i < record.count(); ++i)
		if (record.fieldName(i) == "personID")
		{
			personColumnName = "personID";
			break;
		}
		else if (model->tableName() == "Person" && record.fieldName(i) == "ID")
		{
			personColumnName = "ID";
			break;
		}
		else if (record.fieldName(i) == "debtID")
		{
			debtColumnName = "debtID";
			// don't break incase personID is also present
		}

	// check for filter ID
	std::optional<int64_t> filterID;
	if (toggleNameSelect->isChecked())
		if (nameSelect->currentIndex() != -1)
			filterID = nameSelect->currentData().value<entry::Person>().personEntryID;

	// filter if needed
	if (filterID.has_value())
	{
		if (personColumnName.has_value())
			model->setFilter(personColumnName.value() + QString(" = %1").arg(filterID.value()));
		else if (debtColumnName.has_value())
			model->setFilter(QString("debtID IN (SELECT ID FROM Debt WHERE personID = %1)").arg(filterID.value()));
	}

	model->select();
}

void ManualTab::displayTable()
{
	tableView->setModel(model);
	int idColumnIndex = model->fieldIndex("ID");
	if (idColumnIndex != -1) tableView->setItemDelegateForColumn(idColumnIndex, new ReadOnlyDelegate(tableView)); // set ID column to read-only
	tableView->show();
}

void ManualTab::apply()
{
	model->submitAll();
	model->select();
	unhideAllRows();
	
	Q_EMIT temporaryChangesExist(false);
}

void ManualTab::addTableEntry()
{
	QSqlRecord newRecord = model->record();
	
	int row = -1; // if no row is selected, add at the end
	const QModelIndex idx = tableView->currentIndex();
	if (idx.isValid())
		row = idx.row()+1;

	model->insertRecord(row, newRecord);
}

void ManualTab::deleteTableEntry()
{
	const QModelIndex idx = tableView->currentIndex();
	if (!idx.isValid()) return;
	const int row = idx.row();
	if (!model->removeRow(row)) return;

	tableView->setRowHidden(row, true);
	tableView->setCurrentIndex(QModelIndex());
}

void ManualTab::unhideAllRows()
{
	for (int r = 0; r < model->rowCount(); ++r)
		tableView->setRowHidden(r, false);
}