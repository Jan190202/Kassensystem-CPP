#include "AddTabEntry.h"

#include <QComboBox>;
#include <QLabel>;
#include <QSpinBox>;
#include <QDoubleSpinBox>;
#include <QPushButton>;
#include <vector>
#include <QHBoxLayout>

AddTabEntry::AddTabEntry(QList<QString> nameList, QWidget* parent) : QWidget(parent)
{
	nameSelect = new QComboBox();
	nameSelect->insertItems(0, nameList);

	labelBeer05 = new QLabel();
	labelBeer05->setText("Bier 0,5l (x)");
	spinboxBeer05 = new QSpinBox();
	spinboxBeer05->setMinimum(0);
	spinboxBeer05->setValue(0);

	labelBeer04 = new QLabel();
	labelBeer04->setText("Bier 0,4l (x)");
	spinboxBeer04 = new QSpinBox();
	spinboxBeer04->setMinimum(0);
	spinboxBeer04->setValue(0);

	labelWater = new QLabel();
	labelWater->setText("Wasser (x)");
	spinboxWater = new QSpinBox();
	spinboxWater->setMinimum(0);
	spinboxWater->setValue(0);

	labelSoftdrinks = new QLabel();
	labelSoftdrinks->setText("Softdrinks (x)");
	spinboxSoftdrinks = new QSpinBox();
	spinboxSoftdrinks->setMinimum(0);
	spinboxSoftdrinks->setValue(0);

	labelCustom = new QLabel();
	labelCustom->setText("Sonstiges (€)");
	spinboxCustom = new QDoubleSpinBox();
	spinboxCustom->setMinimum(0.00);
	spinboxCustom->setValue(0);
	spinboxCustom->setDecimals(2);

	labelCost = new QLabel();
	labelCost->setText("0€");

	btnRemove = new QPushButton("-");
	connect(btnRemove, &QPushButton::clicked, this, [this]() {remove(this); });

	entryLayout = new QHBoxLayout();
	entryLayout->addWidget(nameSelect);
	entryLayout->addWidget(labelBeer05);
	entryLayout->addWidget(spinboxBeer05);
	entryLayout->addWidget(labelBeer04);
	entryLayout->addWidget(spinboxBeer04);
	entryLayout->addWidget(labelSoftdrinks);
	entryLayout->addWidget(spinboxSoftdrinks);
	entryLayout->addWidget(labelWater);
	entryLayout->addWidget(spinboxWater);
	entryLayout->addWidget(labelCustom);
	entryLayout->addWidget(spinboxCustom);
	entryLayout->addWidget(labelCost);
	entryLayout->addWidget(btnRemove);
};

AddTabEntry::~AddTabEntry()
{
	delete nameSelect;
	delete labelBeer05;
	delete spinboxBeer05;
	delete labelBeer04;
	delete spinboxBeer04;
	delete labelSoftdrinks;
	delete spinboxSoftdrinks;
	delete labelWater;
	delete spinboxWater;
	delete labelCustom;
	delete spinboxCustom;
	delete labelCost;
	delete btnRemove;
}