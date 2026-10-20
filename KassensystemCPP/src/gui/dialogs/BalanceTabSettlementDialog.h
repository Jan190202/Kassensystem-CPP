#pragma once
#include "gui/types/GuiTypes.h"
#include "domain/model/Entities.h"
#include <QDialog>
#include <string>
#include <optional>

class QDoubleSpinBox;
class QDateEdit;
class QCheckBox;
class QComboBox;
class QPlainTextEdit;
class QCheckBox;

class BalanceTabSettlementDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		double amount;
		std::string comment;
		std::optional<QDate> dateAddedAdvanced;
	};

	BalanceTabSettlementDialog(QWidget* parent);

	BalanceTabSettlementDialog::inputs getInputs() const;

private:
	QDoubleSpinBox*		edtAmount;
	QPlainTextEdit*		edtComment;
	QDateEdit*			edtDate;
	QCheckBox*			edtIsAdvanced;
};