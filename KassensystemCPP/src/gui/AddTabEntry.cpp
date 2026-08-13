#include "AddTabEntry.h"

#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>

AddTabEntry::AddTabEntry(QList<QString> nameList, QWidget* parent)
{
	nameSelect = new QComboBox(parent);
	nameSelect->insertItems(0, nameList);

	spinboxBeer05 = new QSpinBox(parent);
	spinboxBeer05->setMinimum(0);
	spinboxBeer05->setValue(0);

	spinboxBeer04 = new QSpinBox(parent);
	spinboxBeer04->setMinimum(0);
	spinboxBeer04->setValue(0);

	spinboxSoftdrinks = new QSpinBox(parent);
	spinboxSoftdrinks->setMinimum(0);
	spinboxSoftdrinks->setValue(0);

	spinboxWater = new QSpinBox(parent);
	spinboxWater->setMinimum(0);
	spinboxWater->setValue(0);

	spinboxCustom = new QDoubleSpinBox(parent);
	spinboxCustom->setMinimum(0.00);
	spinboxCustom->setValue(0);
	spinboxCustom->setDecimals(2);

	labelCost = new QLabel(parent);
	labelCost->setText("0€");

	btnRemove = new QPushButton("-", parent);
	connect(btnRemove, &QPushButton::clicked, this, [this]() { remove(this); });
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