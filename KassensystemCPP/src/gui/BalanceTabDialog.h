#pragma once

#include <QDialog>
#include <string>
#include <QDate>

class QDateEdit;
class QLineEdit;
class QDoubleSpinBox;

struct dlgInputs
{
	std::string name;
	double cost;
	QDate date;
	std::string comment;
};

class BalanceTabDialog : public QDialog
{
	Q_OBJECT
public:
	BalanceTabDialog(const std::string& dlgName, QWidget* parent);
	dlgInputs& getInputs() const;
private:
	QLineEdit* edtName;
	QDoubleSpinBox* edtCost;
	QDateEdit* edtDate;
	QLineEdit* edtComment;
};