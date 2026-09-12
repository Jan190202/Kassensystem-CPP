#include "AddTab.h"
#include "AddTabEntry.h"
#include "GuiTypes.h"
#include "qtutils/QtConversions.h"
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
#include <QMessageBox>
#include <QDebug>
#include <algorithm>
#include <string>

AddTab::AddTab(const LowerButtonBundle& lowerButtons, ConsumptionService& consumptionService, PersonRepository* personRepo, QWidget* parent) : lowerButtons(lowerButtons), consumptionService(consumptionService), personRepo(personRepo), BaseTab(parent) {}

void AddTab::initialize()
{
	monthSelection = new QDateEdit(QDate::currentDate().addMonths(-1), this);
	monthSelection->setDisplayFormat(QStringLiteral("MMMM yy"));
	monthSelection->setCalendarPopup(false);
	monthSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	btnAddEntry = new QPushButton(QStringLiteral("+ Eintrag hinzufügen"), this);
	btnAddEntry->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	entriesGrid = new QGridLayout();
	entriesGrid->setContentsMargins(6, 0, 6, 0);
	entriesGrid->setHorizontalSpacing(12);
	entriesGrid->setVerticalSpacing(10);

	const QStringList headers = {
		QStringLiteral("Name"),
		QStringLiteral("Bier 0,5 l"),
		QStringLiteral("Bier 0,4 l"),
		QStringLiteral("Softdrinks"),
		QStringLiteral("Wasser"),
		QStringLiteral("Sonstiges"),
		QStringLiteral("Kosten"),
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

	auto* monthLayout = new QHBoxLayout();
	monthLayout->setContentsMargins(0, 0, 0, 0);
	monthLayout->setSpacing(10);
	monthLayout->addWidget(new QLabel(QStringLiteral("Abrechnungsmonat"), this));
	monthLayout->addWidget(monthSelection, 1);

	addTabMainLayout = new QVBoxLayout(this);
	addTabMainLayout->setContentsMargins(18, 18, 18, 18);
	addTabMainLayout->setSpacing(14);

	addTabMainLayout->addLayout(monthLayout);
	addTabMainLayout->addLayout(entriesGrid);
	addTabMainLayout->addWidget(btnAddEntry);
	addTabMainLayout->addStretch();

	btnAddEntry->setFocus(Qt::TabFocusReason);

	connect(btnAddEntry, &QPushButton::clicked, this, &AddTab::addEntry);
	connect(lowerButtons.btnApply, &QPushButton::clicked, this, &AddTab::apply);
}

void AddTab::refresh()
{
	clearEntries();
}

void AddTab::clearEntries()
{
	while (!entries.empty())
	{
		AddTabEntry* entry = entries.back();
		removeEntry(entry);
	}
}

void AddTab::addEntry()
{
	std::vector<entry::Person> personVec = personRepo->getAllPersonEntries();

	auto* newEntry = new AddTabEntry(personVec, this);

	newEntry->addToGrid(entriesGrid, static_cast<int>(entries.size()) + 1);

	entries.push_back(newEntry);

	connect(newEntry, &AddTabEntry::remove,
		this, &AddTab::removeEntry);
	connect(newEntry, &AddTabEntry::calcEntryCost, this, [this](ConsumptionInputs& inputs, double& entryCost)
		{
			request::Consumption request{
				.nBeer05 = inputs.nBeer05,
				.nBeer04 = inputs.nBeer04,
				.nSoftdrinks = inputs.nSoftdrinks,
				.nWater = inputs.nWater,
				.otherExpense = inputs.otherExpense };
			entryCost = consumptionService.calculateDebt(request);
		});

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

void AddTab::apply()
{
	const std::vector<AddTabEntry*> snapshot = entries; // stable copy of pointers for safe entry removal in loop

	for (auto* entry : snapshot)
	{
		ConsumptionInputs inputs = entry->getEntryInputs();

		QDate setDate = monthSelection->date();
		int nDays = setDate.daysInMonth();
		int setDays = setDate.day();
		QDate dateAtMonthEnd = setDate.addDays(nDays - setDays);

		request::Consumption request{
			.personInput = inputs.personInput,
			.date = dateAtMonthEnd,
			.nBeer05 = inputs.nBeer05,
			.nBeer04 = inputs.nBeer04,
			.nSoftdrinks = inputs.nSoftdrinks,
			.nWater = inputs.nWater,
			.otherExpense = inputs.otherExpense };

		const auto requestValidity = consumptionService.isRequestValid(request);

		if (requestValidity.has_value())
		{
			removeEntry(entry);
			consumptionService.addConsumption(request);
		}
		else
		{
			handleInputValidityError(requestValidity.error());
		}
	}
}

void AddTab::save()
{
	apply();
	// TBD
}

void AddTab::handleInputValidityError(const validityError::Code& errorCode) const
{
	using namespace validityError;

	QString errorQStr;
	if (std::holds_alternative<Name>(errorCode))
	{
		errorQStr += QStringLiteral("Name: ");
		switch (std::get<Name>(errorCode))
		{
		case Name::FirstOrLastNameMissing:
		{
			errorQStr += QStringLiteral("Vor- oder Nachname fehlt");
			break;
		}
		case Name::InvalidNicknameFormat:
		{
			errorQStr += QStringLiteral("Fehlerhafter Spitzname");
			break;
		}
		case Name::TooManyComponents:
		{
			errorQStr += QStringLiteral("Zu viele Komponenten");
			break;
		}
		case Name::UnbalancedParentheses:
		{
			errorQStr += QStringLiteral("Fehlerhafte Klammerung");
			break;
		}
		}
		errorQStr += QStringLiteral("\nSyntax: Vorname Nachname ([\"Spitzname\"], [Info])");
	}
	else if (std::holds_alternative<Date>(errorCode))
	{
		errorQStr += QStringLiteral("Datum: ");
		switch (std::get<Date>(errorCode))
		{
		case Date::DateLaterThanCurrentDate:
		{
			errorQStr += QStringLiteral("Zukünftiges Datum eingegeben");
			break;
		}
		}
	}
	else if (std::holds_alternative<Consumption>(errorCode))
	{
		errorQStr += QStringLiteral("Verbrauch: ");
		switch (std::get<Consumption>(errorCode))
		{
		case Consumption::EmptyConsumptionEntries:
		{
			errorQStr += QStringLiteral("Kein Verbrauch eingegeben");
			break;
		}
		case Consumption::SomeEntriesSmallerThanZero:
		{
			errorQStr += QStringLiteral("Teilweise negative Verbräuche");
			break;
		}
		}
	}

	auto* errorDlg = new QMessageBox(QMessageBox::Warning, QStringLiteral("Fehler"), errorQStr);
	errorDlg->exec();
}