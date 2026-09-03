#pragma once

#include "GuiTypes.h"

#include <QDialog>
#include <string>
#include <QDate>
#include <vector>
#include <optional>

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
	std::optional<int64_t> coveringPersonID;
};

class BalanceTabDialog : public QDialog
{
	Q_OBJECT
public:
	BalanceTabDialog(BtnIndex mode, std::vector<Person> personVec, QWidget* parent);
	dlgInputs& getInputs() const;
private:
	QLineEdit*		edtDescription;
	QDoubleSpinBox*	edtCost;
	QDateEdit*		edtDate;
	QPlainTextEdit* edtComment;
	QCheckBox*		edtIsCovered;
	QComboBox*		edtCoveringPerson;
};