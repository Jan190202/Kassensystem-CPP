#include "AddTab.h"

#include "AddTabEntry.h"
#include <QDateEdit>
#include <QPushButton>
#include <QDate>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <algorithm>
#include <string> 
#include <QList>
#include <QFrame>

#include "Utils.h"

AddTab::AddTab(QWidget* parent) : BaseTab(parent) {}

void AddTab::initialize()
{
	monthSelection = new QDateEdit(QDate::currentDate().addMonths(-1));
	monthSelection->setDisplayFormat("MMMM yy");

	btnAddEntry = new QPushButton("+");

	entriesGrid = new QGridLayout(); 

	QFrame* hLine = new QFrame();
	hLine->setFrameShape(QFrame::HLine);

	std::string eurSymbol = "\u20AC";
	entriesGrid->addWidget(new QLabel(tr("Name")), 0, 0, Qt::AlignCenter);
	entriesGrid->addWidget(new QLabel(QString::fromUtf8("Bier 0,5l")), 0, 1, Qt::AlignCenter);
	entriesGrid->addWidget(new QLabel(QString::fromUtf8("Bier 0,4l")), 0, 2, Qt::AlignCenter);
	entriesGrid->addWidget(new QLabel(QString::fromUtf8("Softdrinks")), 0, 3, Qt::AlignCenter);
	entriesGrid->addWidget(new QLabel(QString::fromUtf8("Wasser")), 0, 4, Qt::AlignCenter);
	entriesGrid->addWidget(new QLabel(QString::fromUtf8("Sonstiges ("+eurSymbol+")")), 0, 5, Qt::AlignCenter);
	entriesGrid->addWidget(new QLabel(QString::fromUtf8("Kosten")), 0, 6, Qt::AlignCenter);
	entriesGrid->addWidget(hLine, 1, 0, 1, 7);
	

	AddTabMainLayout = new QVBoxLayout(this);
	AddTabMainLayout->addWidget(monthSelection);
	AddTabMainLayout->addLayout(entriesGrid);
	AddTabMainLayout->addWidget(btnAddEntry);
	AddTabMainLayout->addStretch();

	connect(btnAddEntry, &QPushButton::clicked, this, &AddTab::addEntry);
}

void AddTab::addEntry()
{
	std::vector<std::string> nameList = {"Jane Doe", "John Doe"};
	QList<QString> nameListQ = Utils::strVecToQStrList(nameList);

	AddTabEntry* newEntry = new AddTabEntry(nameListQ, this);
	int row = entriesGrid->rowCount(); 
	newEntry->addToGrid(entriesGrid, row);

	entries.push_back(newEntry);
	connect(newEntry, &AddTabEntry::remove, this, &AddTab::removeEntry);
}

void AddTab::removeEntry(AddTabEntry* entryRemoved)
{
	auto it = std::find(entries.begin(), entries.end(), entryRemoved);
	if (it == entries.end())
		return;

	int removedIndex = static_cast<int>(std::distance(entries.begin(), it));
	int removedRow = removedIndex + 1; 

	entries.erase(it);
	delete entryRemoved;

	int rowCount = entriesGrid->rowCount();
	int columnCount = entriesGrid->columnCount();
	for (int row = removedRow + 1; row < rowCount; ++row)
	{
		for (int col = 0; col < columnCount; ++col)
		{
			QLayoutItem* item = entriesGrid->itemAtPosition(row, col);
			if (!item) continue;
			entriesGrid->removeItem(item);
			entriesGrid->addItem(item, row - 1, col);
		}
	}
}

void AddTab::clearEntries()
{
	while (!entries.empty())
	{
		removeEntry(entries.back());
	}
}