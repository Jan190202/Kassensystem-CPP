#include "gui/dialogs/BalanceTabCalculator.h"
#include "gui/IconLoader.h"
#include "qtutils/QtConversions.h"
#include <QString>
#include <QStringList>
#include <string>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpression>

BalanceTabCalculator::BalanceTabCalculator(QWidget* parent) : QDialog(parent) 
{
	setWindowIcon(iconLoader::getIcon("calculator.png"));
	setWindowTitle("Taschenrechner");

	edtText = new QPlainTextEdit();

	lResultText = new QLabel("Ergebnis:");
	lResultValue = new QLabel(qtUtils::toCurrencyFormat(0.0));

	auto* resultLayout = new QHBoxLayout();
	resultLayout->addWidget(lResultText);
	resultLayout->addSpacing(30);
	resultLayout->addWidget(lResultValue);
	resultLayout->addStretch();

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(edtText);
	mainLayout->addLayout(resultLayout);

	connect(edtText, &QPlainTextEdit::textChanged, this, [=]() {refreshResult(); });
}

void BalanceTabCalculator::refreshResult()
{
	lResultValue->setText(qtUtils::toCurrencyFormat(calculateResult()));
}

double BalanceTabCalculator::calculateResult() const
{
	static const QRegularExpression whitespace(QStringLiteral("\\s+"));
	static const QRegularExpression numberPattern(QStringLiteral("^(\\d+[.,]?\\d*|[.,]\\d+)$"));

	const QStringList tokens = edtText->toPlainText().split(whitespace, Qt::SkipEmptyParts);

	double result{};
	QChar op = '+';          // default operator
	bool opPending = false;  // an explicit operator was read, number still missing

	for (const QString& token : tokens) 
	{
		QString number = token;

		// Token starts with an operator: "+", "-5", "*2", ...
		if (QStringLiteral("+-*/").contains(token.at(0))) {
			if (opPending)
				return 0.0;              // two operators in a row
			op = token.at(0);
			opPending = true;
			number = token.mid(1);
			if (number.isEmpty())
				continue;                // operator stands alone, number comes in the next token
		}

		// Validate strictly, then normalize the decimal separator
		if (!numberPattern.match(number).hasMatch())
			return 0.0;                  // unforeseen component (text, "1e5", "nan", ...)
		number.replace(',', '.');

		bool ok = false;
		const double value = number.toDouble(&ok);
		if (!ok)
			return 0.0;

		switch (op.unicode()) {
		case '+': result += value; break;
		case '-': result -= value; break;
		case '*': result *= value; break;
		case '/':
			if (value == 0.0)
				return 0.0;              // division by zero
			result /= value;
			break;
		}

		op = '+';                        // back to the default
		opPending = false;
	}

	if (opPending)
		return 0.0;                      // input ends with a dangling operator

	return result;
}