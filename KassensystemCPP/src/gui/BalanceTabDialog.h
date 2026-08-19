#pragma once

#include "GuiTypes.h"

#include <QDialog>
#include <string>
#include <QDate>

class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;

struct dlgInputs
{
	std::string description;
	double cost;
	QDate date;
	std::string comment;
	bool isCovered = false;
	std::string coveringPerson;
};

class BalanceTabDialog : public QDialog
{
	Q_OBJECT
public:
	BalanceTabDialog(btnIndex mode, QWidget* parent);
	dlgInputs& getInputs() const;
private:
	QLineEdit*		edtDescription;
	QDoubleSpinBox*	edtCost;
	QDateEdit*		edtDate;
	QPlainTextEdit* edtComment;
	QCheckBox*		edtIsCovered;
	QComboBox*		edtCoveringPerson;
};