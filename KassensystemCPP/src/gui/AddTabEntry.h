#pragma once
#include "GuiTypes.h"
#include "qtutils/QtConversions.h"
#include <QObject>
#include <QList>
#include <QString>

class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QWidget;

class AddTabEntry : public QObject
{
	Q_OBJECT

signals:
	void remove(AddTabEntry* removedEntry);
	void calcEntryCost(ConsumptionInputs& inputs, double& cost);

public:
	AddTabEntry(std::vector<entry::Person>& personVec, QWidget* parent = nullptr);
	~AddTabEntry() override;
	void addToGrid(QGridLayout* grid, int row);
	void removeFromGrid(QGridLayout* grid);

	QWidget* getLastWidget() const;
	ConsumptionInputs getEntryInputs() const;

private:
	QComboBox* nameSelect = nullptr;
	QSpinBox* spinboxBeer05 = nullptr;
	QSpinBox* spinboxBeer04 = nullptr;
	QSpinBox* spinboxSoftdrinks = nullptr;
	QSpinBox* spinboxWater = nullptr;
	QDoubleSpinBox* spinboxCustom = nullptr;
	QLabel* lCost = nullptr;
	QPushButton* btnRemove = nullptr;

	double entryCost{};
};