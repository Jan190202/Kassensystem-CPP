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

class BalanceTabDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		std::string description;
		double amount;
		RegisterDate date;
		std::string comment;
		std::optional<int64_t> coveringpersonEntryID;
	};

	BalanceTabDialog(BtnIndex mode, std::vector<entry::Person>& personVec, QWidget* parent);

	BalanceTabDialog::inputs getInputs() const;

private:
	QLineEdit*		edtDescription;
	QDoubleSpinBox*	edtCost;
	QDateEdit*		edtDate;
	QCheckBox*		edtIsSpecial;
	QComboBox*		edtSpecial;
	QPlainTextEdit* edtComment;
	QCheckBox*		edtIsCovered;
	QComboBox*		edtCoveringPerson;
};