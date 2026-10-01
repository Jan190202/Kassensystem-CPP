#pragma once
#include "ManualTab.h"

#include <QComboBox>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>

ManualTab::ManualTab(const LowerButtonBundle& lowerButtons, const RepositoryBundle& repoBundle, QSqlDatabase& db, QWidget* parent)
	: lowerButtons(lowerButtons), repoBundle(repoBundle), db(db), BaseTab(parent) {}

void ManualTab::initialize()
{
	tableSelect = new QComboBox();
	tableSelect->setMinimumWidth(300);
	auto* filterLayout = new QHBoxLayout();
	filterLayout->addWidget(tableSelect);
	filterLayout->addStretch();

	tableView = new QTableView();

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(filterLayout);
	mainLayout->addWidget(tableView);
}

void ManualTab::refresh()
{

}

void ManualTab::apply()
{

}