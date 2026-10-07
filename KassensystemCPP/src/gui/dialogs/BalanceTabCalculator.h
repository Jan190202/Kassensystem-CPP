#pragma once
#include <QDialog>

class QPlainTextEdit;
class QLabel;

class BalanceTabCalculator : public QDialog
{
	Q_OBJECT
public:
	BalanceTabCalculator(QWidget* parent);

private:
	void refreshResult();
	double calculateResult() const;

	QPlainTextEdit* edtText;
	QLabel* lResultText;
	QLabel* lResultValue;
};