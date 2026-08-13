#pragma once
#include <QWidget>
#include <vector>
#include <string>

class QComboBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QHBoxLayout;

class AddTabEntry : public QWidget
{
	Q_OBJECT
signals:
	void remove(AddTabEntry* removedEntry);
public:
	AddTabEntry(QList<QString> nameList = { tr("") }, QWidget* parent = nullptr);
	~AddTabEntry();
	QHBoxLayout* entryLayout;
private:
	

	QComboBox* nameSelect;

	QLabel* labelBeer05;
	QSpinBox* spinboxBeer05;

	QLabel* labelBeer04;
	QSpinBox* spinboxBeer04;

	QLabel* labelSoftdrinks;
	QSpinBox* spinboxSoftdrinks;

	QLabel* labelWater;
	QSpinBox* spinboxWater;

	QLabel* labelCustom;
	QDoubleSpinBox* spinboxCustom;

	QLabel* labelCost;

	QPushButton* btnRemove;
};