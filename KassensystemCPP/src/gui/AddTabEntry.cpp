#include "AddTabEntry.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QWidget>
#include <QCompleter>

AddTabEntry::AddTabEntry(const QList<QString>& nameList, QWidget* parent)
{
	nameSelect = new QComboBox(parent);
	nameSelect->addItems(nameList);
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
	spinboxCustom->setPrefix(QStringLiteral("€ "));
	spinboxCustom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	labelCost = new QLabel(QStringLiteral("0,00 €"), parent);
	labelCost->setAlignment(Qt::AlignCenter);
	labelCost->setMinimumWidth(70);

	btnRemove = new QPushButton(QStringLiteral("−"), parent);
	btnRemove->setToolTip(tr("Eintrag entfernen"));
	btnRemove->setFixedWidth(36);

	connect(btnRemove, &QPushButton::clicked, this, [this]() { emit remove(this); });
	//connect(nameSelect, &QComboBox::)
}

AddTabEntry::~AddTabEntry()
{
	delete nameSelect;
	delete spinboxBeer05;
	delete spinboxBeer04;
	delete spinboxSoftdrinks;
	delete spinboxWater;
	delete spinboxCustom;
	delete labelCost;
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
	grid->addWidget(labelCost, row, 6);
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
	grid->removeWidget(labelCost);
	grid->removeWidget(btnRemove);
}

//QWidget* AddTabEntry::getFirstWidget()
//{
//	return nameSelect;
//}

QWidget* AddTabEntry::getLastWidget()
{
	return btnRemove;
}