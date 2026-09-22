#include "PayTabAddCreditDialog.h"
#include "qtutils/QtConversions.h"
#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QDate>
#include <QFrame>
#include <QFont>
#include <QPlainTextEdit>
#include <string>

PayTabAddCreditDialog::PayTabAddCreditDialog(QWidget* parent) : QDialog(parent)
{
	QFont boldFont = font();
	boldFont.setBold(true);


	edtAmount = new QDoubleSpinBox();
	edtAmount->setRange(0.0, 1'000'000.0);
	edtAmount->setDecimals(2);
	edtAmount->setSuffix(QStringLiteral(" ") + QtUtils::eurSymbol());
	edtAmount->setSingleStep(1.0);
	edtAmount->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	edtDate = new QDateEdit(QDate::currentDate());
	edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	edtDate->setCalendarPopup(true);

	edtDescription = new QLineEdit();

	auto* lblAmount = new QLabel(QStringLiteral("Neues Guthaben:"));
	lblAmount->setFont(boldFont);
	auto* lblDate = new QLabel(QStringLiteral("Datum:"));
	lblDate->setFont(boldFont);
	auto* lblDescription = new QLabel(QStringLiteral("Beschreibung:"));
	lblDescription->setFont(boldFont);

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblAmount, edtAmount);
	form->addRow(lblDate, edtDate);
	form->addRow(lblDescription, edtDescription);

	auto* bottomSeparator = new QFrame;
	bottomSeparator->setFrameShape(QFrame::HLine);
	bottomSeparator->setFrameShadow(QFrame::Sunken);

	auto* btnOK = new QPushButton(QStringLiteral("OK"));
	auto* btnCancel = new QPushButton(QStringLiteral("Cancel"));
	btnOK->setDefault(true);
	btnOK->setMinimumWidth(90);
	btnCancel->setMinimumWidth(90);

	auto* btnLayout = new QHBoxLayout;
	btnLayout->addStretch();
	btnLayout->addWidget(btnCancel);
	btnLayout->addWidget(btnOK);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(24, 20, 24, 16);
	mainLayout->setSpacing(12);
	mainLayout->addLayout(form);
	mainLayout->addSpacing(8);
	mainLayout->addStretch();
	mainLayout->addWidget(bottomSeparator);
	mainLayout->addLayout(btnLayout);

	connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

	setMinimumWidth(520);
	adjustSize();
}

PayTabAddCreditDialog::inputs PayTabAddCreditDialog::getInputs() const
{
	return PayTabAddCreditDialog::inputs{
		.amount = edtAmount->value(),
		.date = edtDate->date(),
		.description = edtDescription->text().toStdString()
	};
}