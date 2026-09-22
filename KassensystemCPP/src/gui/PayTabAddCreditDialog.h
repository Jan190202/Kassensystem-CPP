#pragma once
#include <QDialog>
#include <QDate>
#include <string>

class QDoubleSpinBox;
class QDateEdit;
class QLineEdit;

class PayTabAddCreditDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		double amount;
		QDate date;
		std::string description;
	};

	PayTabAddCreditDialog(QWidget* parent);

	PayTabAddCreditDialog::inputs getInputs() const;

private:
	QDoubleSpinBox* edtAmount;
	QDateEdit*		edtDate;
	QLineEdit*		edtDescription;
};