#pragma once
#include <QDialog>

class QPlainTextEdit;
class QLabel;

class BalanceTabReviewDialog : public QDialog
{
	Q_OBJECT
public:
	struct inputs
	{
		double countedCash;
		double expectedCash;
		std::string comment;
	};

	BalanceTabReviewDialog(double expectedCash, QWidget* parent);

	BalanceTabReviewDialog::inputs getInputs() const;

private:
	void refreshResult();
	double calculateResult() const;

	QPlainTextEdit* edtText;
	QLabel* lResultText;
	QLabel* lResultValue;
};