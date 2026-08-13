#pragma once
#include "BaseTab.h"

class QComboBox;
class QLabel;
class QTableWidget;
class QCheckBox;
class QDoubleSpinBox;

class PayTab : public BaseTab
{
	Q_OBJECT
public:
	PayTab(QWidget* parent = nullptr);
	virtual void initialize() override;
private:
	QComboBox* nameSelect = nullptr;
	QLabel* totalLabel = nullptr;
	QLabel* paidLabel = nullptr;
	QLabel* dueLabel = nullptr;
	QLabel* creditLabel = nullptr;
	QDoubleSpinBox* paymentSpinBox = nullptr;
	QCheckBox* fullPaymentCheckBox = nullptr;
	QCheckBox* creditCheckBox = nullptr;
	QCheckBox* tipCheckBox = nullptr;
	QTableWidget* consumptionTable = nullptr; //prototype, will be replaced by QTableView and database in the future
};