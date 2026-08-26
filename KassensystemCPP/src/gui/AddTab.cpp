#include "AddTab.h"

#include "AddTabEntry.h"
#include "qtutils/QtConversions.h"
#include "GuiTypes.h"
#include "ocr/EntryRecognizer.h"

#include <QDate>
#include <QDateEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QSpacerItem>

#include <algorithm>
#include <string>

#include <QDebug>

AddTab::AddTab(const LowerButtonBundle& lowerButtons, ConsumptionService& consumptionService, QWidget* parent) : lowerButtons(lowerButtons), consumptionService(consumptionService), BaseTab(parent) {}

void AddTab::initialize()
{
	monthSelection = new QDateEdit(QDate::currentDate().addMonths(-1), this);
	monthSelection->setDisplayFormat(QStringLiteral("MMMM yy"));
	monthSelection->setCalendarPopup(false);
	monthSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	btnScanDocument = new QPushButton(tr("Photo"), this);

	btnAddEntry = new QPushButton(tr("+ Eintrag hinzufügen"), this);
	btnAddEntry->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	entriesGrid = new QGridLayout();
	entriesGrid->setContentsMargins(6, 0, 6, 0);
	entriesGrid->setHorizontalSpacing(12);
	entriesGrid->setVerticalSpacing(10);

	const QStringList headers = {
		tr("Name"),
		tr("Bier 0,5 l"),
		tr("Bier 0,4 l"),
		tr("Softdrinks"),
		tr("Wasser"),
		tr("Sonstiges"),
		tr("Kosten"),
		QString()
	};

	for (int column = 0; column < headers.size() - 1; ++column)
	{
		auto* headerLabel = new QLabel(headers.at(column), this);
		headerLabel->setAlignment(Qt::AlignCenter);

		QFont font = headerLabel->font();
		font.setBold(true);
		headerLabel->setFont(font);

		entriesGrid->addWidget(headerLabel, 0, column);
	}

	entriesGrid->setColumnMinimumWidth(0, 140); 
	entriesGrid->setColumnMinimumWidth(1, 105);
	entriesGrid->setColumnMinimumWidth(2, 105);
	entriesGrid->setColumnMinimumWidth(3, 105);
	entriesGrid->setColumnMinimumWidth(4, 105);
	entriesGrid->setColumnMinimumWidth(5, 125); 
	entriesGrid->setColumnMinimumWidth(6, 75); 
	entriesGrid->setColumnMinimumWidth(7, 36);

	entriesGrid->setColumnStretch(0, 2);
	entriesGrid->setColumnStretch(1, 1);
	entriesGrid->setColumnStretch(2, 1);
	entriesGrid->setColumnStretch(3, 1);
	entriesGrid->setColumnStretch(4, 1);
	entriesGrid->setColumnStretch(5, 1);
	entriesGrid->setColumnStretch(6, 1);
	entriesGrid->setColumnStretch(7, 0);

	auto* columnSpacer = new QSpacerItem(36, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
	entriesGrid->addItem(columnSpacer, 0, 7);

	auto* firstRowLayout = new QHBoxLayout();
	firstRowLayout->setContentsMargins(0, 0, 0, 0);
	firstRowLayout->setSpacing(10);
	firstRowLayout->addWidget(new QLabel(tr("Abrechnungsmonat"), this));
	firstRowLayout->addWidget(monthSelection, 1);
	firstRowLayout->addWidget(btnScanDocument);

	addTabMainLayout = new QVBoxLayout(this);
	addTabMainLayout->setContentsMargins(18, 18, 18, 18);
	addTabMainLayout->setSpacing(14);

	addTabMainLayout->addLayout(firstRowLayout);
	addTabMainLayout->addLayout(entriesGrid);
	addTabMainLayout->addWidget(btnAddEntry);
	addTabMainLayout->addStretch();

	btnAddEntry->setFocus(Qt::TabFocusReason);

	connect(btnAddEntry, &QPushButton::clicked, this, [&]() {Q_EMIT addEntry(); }); // lambda needed to emit addEntry() with standard argument
	connect(lowerButtons.btnApply, &QPushButton::clicked, this, &AddTab::apply);
	connect(btnScanDocument, &QPushButton::clicked, this, [&]()
		{
			std::string imagePath;
			Q_EMIT takePicture(imagePath); // pass by reference, TBD
			std::vector<ConsumptionInputs> inputVector = EntryRecognizer::instance().readDocument(imagePath);
			EntryRecognizer::instance().shutdownInterpreter();
			clearEntries();
			for (auto& input : inputVector)
			{
				Q_EMIT addEntry(input);
			}
		});
}

void AddTab::addEntry(const ConsumptionInputs& inputsPredef)
{
	QList<QString> nameListQ = QtUtils::strVecToQStrList(consumptionService.getPersonNames());

	auto* newEntry = new AddTabEntry(nameListQ, inputsPredef, this);

	connect(newEntry, &AddTabEntry::remove, this, &AddTab::removeEntry);
	connect(newEntry, &AddTabEntry::calcEntryCost, this, [this](const ConsumptionInputs& inputs, double& entryCost)
		{
			ConsumptionRequest request{
				.nBeer05 = inputs.nBeer05,
				.nBeer04 = inputs.nBeer04,
				.nSoftdrinks = inputs.nSoftdrinks,
				.nWater = inputs.nWater,
				.otherExpense = inputs.otherExpense };
			entryCost = consumptionService.calculateDebt(request);
		});
	newEntry->refreshCost();

	newEntry->addToGrid(entriesGrid, static_cast<int>(entries.size()) + 1);
	entries.push_back(newEntry);

	// set tabulator switch from last widget of lowest entry to addButton
	QWidget::setTabOrder(newEntry->getLastWidget(), btnAddEntry);
}

void AddTab::removeEntry(AddTabEntry* entry)
{
	const auto it = std::find(entries.begin(), entries.end(), entry);
	if (it == entries.end())
		return;

	entry->removeFromGrid(entriesGrid);
	entries.erase(it);
	delete entry;

	shiftEntries();
}

void AddTab::shiftEntries()
{
	for (int index = 0; index < static_cast<int>(entries.size()); ++index)
	{
		entries[index]->removeFromGrid(entriesGrid);
		entries[index]->addToGrid(entriesGrid, index + 1);
	}
}

void AddTab::clearEntries()
{
	while (!entries.empty())
	{
		AddTabEntry* entry = entries.back();
		removeEntry(entry);
	};
}

void AddTab::apply()
{
	while (!entries.empty())
	{
		AddTabEntry* entry = entries.back();
		ConsumptionInputs inputs = entry->getEntryInputs();
		removeEntry(entry);

		// get last day of month
		QDate setDate = monthSelection->date();
		int nDays = setDate.daysInMonth();
		int setDays = setDate.day();
		QDate dateAtMonthEnd = setDate.addDays(nDays - setDays);

		ConsumptionRequest request{
			.personName = inputs.personName,
			.date = dateAtMonthEnd,
			.nBeer05 = inputs.nBeer05,
			.nBeer04 = inputs.nBeer04,
			.nSoftdrinks = inputs.nSoftdrinks,
			.nWater = inputs.nWater,
			.otherExpense = inputs.otherExpense };

		consumptionService.addConsumption(request);
	}
}

void AddTab::save()
{
	apply();
	//consumptionService->save();
	//consumptionService->sync();
}