#include "AddTab.h"

#include "AddTabEntry.h"
#include <QDateEdit>
#include <QPushButton>
#include <QDate>
#include <QVBoxLayout>

AddTab::AddTab(QWidget* parent) : BaseTab(parent) {}

void AddTab::initialize()
{
	// set last month to initial selection
	QDate today = QDate();
	monthSelection = new QDateEdit(today.currentDate().addMonths(-1));
	monthSelection->setDisplayFormat("MMMM yy");

	btnAddEntry = new QPushButton("+");

	AddTabMainLayout = new QVBoxLayout(this);
	AddTabMainLayout->addWidget(monthSelection);
	AddTabMainLayout->addWidget(btnAddEntry);
	AddTabMainLayout->addStretch();

	connect(btnAddEntry, &QPushButton::clicked, this, &AddTab::addEntry);
}

void AddTab::addEntry()
{
	entryCounter++;
	AddTabEntry* newEntry = new AddTabEntry({ tr("") }, this);
	entries.push_back(newEntry);
	AddTabMainLayout->insertLayout(entryCounter, newEntry->entryLayout);

	connect(newEntry, &AddTabEntry::remove, this, &AddTab::removeEntry);
}

void AddTab::removeEntry(AddTabEntry* entryRemoved)
{
	AddTabMainLayout->removeWidget(entryRemoved);
	delete entryRemoved;
	entryCounter--;
}

void AddTab::clearEntries()
{
	for (AddTabEntry* entry : entries)
	{
		removeEntry(entry);
	}
}