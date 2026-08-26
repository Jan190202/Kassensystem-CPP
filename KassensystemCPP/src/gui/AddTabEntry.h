#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include "GuiTypes.h"
#include "qtutils/QtConversions.h"

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

Q_SIGNALS:
	void remove(AddTabEntry* removedEntry);
	void calcEntryCost(ConsumptionInputs&, double&);

public:
	AddTabEntry(const QList<QString>& nameList = { QString() },
		QWidget* parent = nullptr);
	~AddTabEntry() override;

	void addToGrid(QGridLayout* grid, int row);
	void removeFromGrid(QGridLayout* grid);
	ConsumptionInputs getEntryInputs() const;
	//QWidget* getFirstWidget();
	QWidget* getLastWidget();
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