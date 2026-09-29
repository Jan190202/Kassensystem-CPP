#pragma once
#include "GuiTypes.h"
#include "domain/model/Entities.h"
#include <QDialog>
#include <string>

class QDoubleSpinBox;
class QDateEdit;
class QCheckBox;
class QComboBox;
class QPlainTextEdit;

class BalanceTabSettlementDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		double amount;
		std::string comment;
	};

	BalanceTabSettlementDialog(QWidget* parent);

	BalanceTabSettlementDialog::inputs getInputs() const;

private:
	QDoubleSpinBox*		edtAmount;
	QPlainTextEdit*		edtComment;
};