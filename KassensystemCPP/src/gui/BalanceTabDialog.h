#pragma once
#include "GuiTypes.h"
#include "domain/model/Entities.h"
#include <QDialog>
#include <QDate>
#include <vector>
#include <optional>
#include <string>

class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;

struct dlgInputs
{
	std::string description;
	double amount;
	QDate date;
	std::string comment;
	std::optional<int64_t> coveringpersonEntryID;
};

class BalanceTabDialog : public QDialog
{
	Q_OBJECT
public:
	BalanceTabDialog(BtnIndex mode, const std::vector<entry::Person>& personVec, QWidget* parent);

	dlgInputs getInputs() const;

private:
	QLineEdit*		edtDescription;
	QDoubleSpinBox*	edtCost;
	QDateEdit*		edtDate;
	QPlainTextEdit* edtComment;
	QCheckBox*		edtIsCovered;
	QComboBox*		edtCoveringPerson;
};