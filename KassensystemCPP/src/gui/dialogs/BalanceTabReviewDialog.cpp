#include "gui/dialogs/BalanceTabReviewDialog.h"
#include "gui/IconLoader.h"
#include "qtutils/QtConversions.h"
#include <QString>
#include <QStringList>
#include <string>
#include <QLabel>
#include <QPlainTextEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRegularExpression>
#include <QFrame>

BalanceTabReviewDialog::BalanceTabReviewDialog(double expectedCash, QWidget* parent) : QDialog(parent)
{
	setWindowIcon(iconLoader::getIcon("check.png"));
	setWindowTitle("Kassensturz");

	QFont boldFont = font();
	boldFont.setBold(true);

	// inputs
	edtCounted = new QDoubleSpinBox();
	edtCounted->setRange(0.0, 1'000'000.0);
	edtCounted->setDecimals(2);
	edtCounted->setSuffix(QStringLiteral(" ") + qtUtils::eurSymbol());
	edtCounted->setSingleStep(1.0);
	edtCounted->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	edtExpected = new QDoubleSpinBox();
	edtExpected->setRange(0.0, 1'000'000.0);
	edtExpected->setDecimals(2);
	edtExpected->setSuffix(QStringLiteral(" ") + qtUtils::eurSymbol());
	edtExpected->setSingleStep(1.0);
	edtExpected->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	edtExpected->setValue(expectedCash);
	edtExpected->setEnabled(false);

	edtComment = new QPlainTextEdit();

	auto* lblCounted = new QLabel(QStringLiteral("Vorliegender Betrag:"));
	lblCounted->setFont(boldFont);
	auto* lblExpected = new QLabel(QStringLiteral("Erwarteter Betrag:"));
	lblExpected->setFont(boldFont);
	auto* lblComment = new QLabel(QStringLiteral("Kommentar:"));
	lblComment->setFont(boldFont);

	auto* form = new QFormLayout();
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblCounted, edtCounted);
	form->addRow(lblExpected, edtExpected);
	form->addRow(lblComment, edtComment);

	// calculator
	auto* lblCalc = new QLabel(QStringLiteral("Nebenrechnung:"));
	lblCalc->setFont(boldFont);

	edtCalculation = new QPlainTextEdit();

	lblResultText = new QLabel("Ergebnis:");
	lblResultValue = new QLabel(qtUtils::toCurrencyFormat(0.0));

	auto* resultLayout = new QHBoxLayout();
	resultLayout->addWidget(lblResultText);
	resultLayout->addSpacing(30);
	resultLayout->addWidget(lblResultValue);
	resultLayout->addStretch();

	auto* calcLayout = new QVBoxLayout();
	calcLayout->addWidget(lblCalc);
	calcLayout->addWidget(edtCalculation);
	calcLayout->addLayout(resultLayout);

	// horizontal stack
	auto* hSeperator = new QFrame();
	hSeperator->setFrameShape(QFrame::VLine);
	hSeperator->setFrameShadow(QFrame::Sunken);

	auto* hLayout = new QHBoxLayout();
	hLayout->addLayout(form);
	hLayout->addWidget(hSeperator);
	hLayout->addLayout(calcLayout);

	// buttons
	auto* btnOK = new QPushButton(QStringLiteral("OK"));
	auto* btnCancel = new QPushButton(QStringLiteral("Cancel"));
	btnOK->setDefault(true);
	btnOK->setMinimumWidth(90);
	btnCancel->setMinimumWidth(90);

	auto* btnLayout = new QHBoxLayout;
	btnLayout->addStretch();
	btnLayout->addWidget(btnCancel);
	btnLayout->addWidget(btnOK);

	// main layout
	auto* bottomSeparator = new QFrame();
	bottomSeparator->setFrameShape(QFrame::HLine);
	bottomSeparator->setFrameShadow(QFrame::Sunken);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(hLayout);
	mainLayout->addWidget(bottomSeparator);
	mainLayout->addLayout(btnLayout);

	setMinimumWidth(520);
	adjustSize();

	connect(edtCalculation, &QPlainTextEdit::textChanged, this, [=]() {refreshResult(); });
	connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void BalanceTabReviewDialog::refreshResult()
{
	lblResultValue->setText(qtUtils::toCurrencyFormat(calculateResult()));
}

double BalanceTabReviewDialog::calculateResult() const
{
	static const QRegularExpression whitespace(QStringLiteral("\\s+"));
	static const QRegularExpression numberPattern(QStringLiteral("^(\\d+[.,]?\\d*|[.,]\\d+)$"));

	const QStringList tokens = edtCalculation->toPlainText().split(whitespace, Qt::SkipEmptyParts);

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

BalanceTabReviewDialog::inputs BalanceTabReviewDialog::getInputs() const
{
	return BalanceTabReviewDialog::inputs{
		.countedCash = edtCounted->value(),
		.comment = edtComment->toPlainText().toStdString()
	};
}