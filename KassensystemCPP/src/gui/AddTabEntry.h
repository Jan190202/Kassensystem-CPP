#pragma once
#include <QObject>
#include <QList>
#include <QString>

class QComboBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QGridLayout;
class QWidget;

class AddTabEntry : public QObject
{
	Q_OBJECT
signals:
	void remove(AddTabEntry* removedEntry);
public:
	AddTabEntry(QList<QString> nameList = { QString() }, QWidget* parent = nullptr);
	~AddTabEntry();

	void addToGrid(QGridLayout* grid, int row);
	void removeFromGrid(QGridLayout* grid);

private:
	QComboBox* nameSelect;
	QSpinBox* spinboxBeer05;
	QSpinBox* spinboxBeer04;
	QSpinBox* spinboxSoftdrinks;
	QSpinBox* spinboxWater;
	QDoubleSpinBox* spinboxCustom;
	QLabel* labelCost;
	QPushButton* btnRemove;
};