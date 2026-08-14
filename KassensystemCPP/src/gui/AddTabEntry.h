#pragma once

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

public:
	AddTabEntry(const QList<QString>& nameList = { QString() },
		QWidget* parent = nullptr);
	~AddTabEntry() override;

	void addToGrid(QGridLayout* grid, int row);
	void removeFromGrid(QGridLayout* grid);
	//QWidget* getFirstWidget();
	QWidget* getLastWidget();
private:
	QComboBox* nameSelect = nullptr;
	QSpinBox* spinboxBeer05 = nullptr;
	QSpinBox* spinboxBeer04 = nullptr;
	QSpinBox* spinboxSoftdrinks = nullptr;
	QSpinBox* spinboxWater = nullptr;
	QDoubleSpinBox* spinboxCustom = nullptr;
	QLabel* labelCost = nullptr;
	QPushButton* btnRemove = nullptr;
};