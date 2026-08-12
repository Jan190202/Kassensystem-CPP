#pragma once
#include <QWidget>

class QComboBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;

class AddTabEntry : public QWidget
{
	Q_OBJECT
public:
	AddTabEntry(int num, QWidget* parent = nullptr) : QWidget(parent), entryNum(num) {};
private:
	int entryNum;

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

	QPushButton* btnRemove;
};