#include "AddTabEntry.h"
#include "qtutils/QtConversions.h"

#include <variant>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QWidget>
#include <QCompleter>
#include <QLineEdit>

#include <QDebug>

AddTabEntry::AddTabEntry(const std::vector<Person>& personVec, QWidget* parent)
{
	nameSelect = new QComboBox(parent);
	QList<QString> nameList = QtUtils::personVecToQStrList(personVec, &Person::getFullSpecifier);
	for (size_t i = 0; i < personVec.size(); i++) 
		nameSelect->addItem(
			nameList.at(i), 
			QVariant::fromValue(personVec.at(i).getID())
		);
	nameSelect->setEditable(true);
	nameSelect->setDuplicatesEnabled(false);
	auto* dropDownCompleter = new QCompleter(nameList);
	dropDownCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	dropDownCompleter->setCompletionMode(QCompleter::InlineCompletion);
	nameSelect->setCompleter(dropDownCompleter);
	nameSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	spinboxBeer05 = new QSpinBox(parent);
	spinboxBeer04 = new QSpinBox(parent);
	spinboxSoftdrinks = new QSpinBox(parent);
	spinboxWater = new QSpinBox(parent);

	const auto configureCountSpinBox = [](QSpinBox* spinBox)
		{
			spinBox->setMinimum(0);
			spinBox->setMaximum(99);
			spinBox->setValue(0);
			spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		};

	configureCountSpinBox(spinboxBeer05);
	configureCountSpinBox(spinboxBeer04);
	configureCountSpinBox(spinboxSoftdrinks);
	configureCountSpinBox(spinboxWater);

	spinboxCustom = new QDoubleSpinBox(parent);
	spinboxCustom->setMinimum(0.00);
	spinboxCustom->setMaximum(999.99);
	spinboxCustom->setDecimals(2);
	spinboxCustom->setSingleStep(0.5);
	spinboxCustom->setSuffix(QStringLiteral(" €"));
	spinboxCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	lCost = new QLabel(QtUtils::toCurrencyFormat(entryCost), parent);
	lCost->setAlignment(Qt::AlignCenter);
	lCost->setMinimumWidth(70);

	btnRemove = new QPushButton(QStringLiteral("−"), parent);
	btnRemove->setToolTip(tr("Eintrag entfernen"));
	btnRemove->setFixedWidth(36);

	connect(btnRemove, &QPushButton::clicked, this, [this]() { emit remove(this); });

	auto consumptionInputChanged = [&]()
		{
			ConsumptionInputs inputs = getEntryInputs();
			emit calcEntryCost(inputs, entryCost); // entryCost passed by reference for result retrieval
			lCost->setText(QtUtils::toCurrencyFormat(entryCost));
		};

	connect(spinboxBeer05, &QSpinBox::valueChanged, this, consumptionInputChanged);
	connect(spinboxBeer04, &QSpinBox::valueChanged, this, consumptionInputChanged);
	connect(spinboxSoftdrinks, &QSpinBox::valueChanged, this, consumptionInputChanged);
	connect(spinboxWater, &QSpinBox::valueChanged, this, consumptionInputChanged);
	connect(spinboxCustom, &QDoubleSpinBox::valueChanged, this, consumptionInputChanged);

	// setup functionality that the input text of nameSelect is also submitted in focus-loss (editingFinished), not only enter-press
	QLineEdit* nameEdit = nameSelect->lineEdit();
	connect(nameEdit, &QLineEdit::editingFinished, this, [this, nameEdit]()
		{
			if (!nameEdit->isModified())
				return; // already handled this text, avoid duplicate on enter-press and focus-loss

			const QString text = nameSelect->currentText();
			int index = nameSelect->findText(text);

			if (!text.isEmpty() && index == -1) // avoid empty strings and duplicates
			{
				nameSelect->addItem(text);
				index = nameSelect->count() - 1; // index of the item just added
			}

			nameSelect->setCurrentIndex(index);

			nameEdit->setModified(false); // mark as handled
		});
}

AddTabEntry::~AddTabEntry()
{
	delete nameSelect;
	delete spinboxBeer05;
	delete spinboxBeer04;
	delete spinboxSoftdrinks;
	delete spinboxWater;
	delete spinboxCustom;
	delete lCost;
	delete btnRemove;
}

void AddTabEntry::addToGrid(QGridLayout* grid, int row)
{
	grid->addWidget(nameSelect, row, 0);
	grid->addWidget(spinboxBeer05, row, 1);
	grid->addWidget(spinboxBeer04, row, 2);
	grid->addWidget(spinboxSoftdrinks, row, 3);
	grid->addWidget(spinboxWater, row, 4);
	grid->addWidget(spinboxCustom, row, 5);
	grid->addWidget(lCost, row, 6);
	grid->addWidget(btnRemove, row, 7);
}

void AddTabEntry::removeFromGrid(QGridLayout* grid)
{
	grid->removeWidget(nameSelect);
	grid->removeWidget(spinboxBeer05);
	grid->removeWidget(spinboxBeer04);
	grid->removeWidget(spinboxSoftdrinks);
	grid->removeWidget(spinboxWater);
	grid->removeWidget(spinboxCustom);
	grid->removeWidget(lCost);
	grid->removeWidget(btnRemove);
}

ConsumptionInputs AddTabEntry::getEntryInputs() const
{
	std::variant<int64_t, std::string> personInput;
	QVariant currentData = nameSelect->currentData();
	if (currentData.isValid())
	{
		personInput = currentData.toLongLong();
	}
	else
	{
		personInput = nameSelect->currentText().toStdString();
	}

	return ConsumptionInputs{
		.personInput = personInput,
		.nBeer05 = spinboxBeer05->value(),
		.nBeer04 = spinboxBeer04->value(),
		.nSoftdrinks = spinboxSoftdrinks->value(),
		.nWater = spinboxWater->value(),
		.otherExpense = spinboxCustom->value() };
}

//QWidget* AddTabEntry::getFirstWidget()
//{
//	return nameSelect;
//}

QWidget* AddTabEntry::getLastWidget()
{
	return btnRemove;
}