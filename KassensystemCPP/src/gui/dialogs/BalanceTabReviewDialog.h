#pragma once
#include <QDialog>

class QPlainTextEdit;
class QLabel;
class QDoubleSpinBox;
class QPushButton;

class BalanceTabReviewDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		double countedCash;
		std::string comment;
	};

	BalanceTabReviewDialog(QWidget* parent, double expectedCash = 0.0);

	BalanceTabReviewDialog::inputs getInputs() const;

private:
	void refreshResult();
	double calculateResult() const;

	QPlainTextEdit* edtCalculation;
	QLabel* lblResultText;
	QLabel* lblResultValue;
	QDoubleSpinBox* edtCounted;
	QDoubleSpinBox* edtExpected;
	QPlainTextEdit* edtComment;
	QPushButton* btnOK;
	QPushButton* btnCancel;
};