#include "BalanceTabDialog.h"

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

#include <string>

BalanceTabDialog::BalanceTabDialog(const std::string& dlgName, QWidget* parent) : QDialog(parent)
{
	QString dlgNameQ = QString::fromStdString(dlgName);

	resize(420, 380);
	setWindowTitle(dlgNameQ);

	edtName = new QLineEdit();
	edtName->setPlaceholderText(tr("z. B. Gehalt, Verkauf ..."));

	edtCost = new QDoubleSpinBox();
	edtCost->setRange(0.0, 1'000'000.0);
	edtCost->setDecimals(2);
	edtCost->setSuffix(QStringLiteral(" €"));
	edtCost->setSingleStep(1.0);
	edtCost->setAlignment(Qt::AlignRight);

	edtDate = new QDateEdit(QDate::currentDate());
	edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	edtDate->setCalendarPopup(true);

	edtComment = new QLineEdit();
	edtComment->setPlaceholderText(tr("optional"));

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows); 

	form->addRow(new QLabel(tr("<b>Bezeichnung:</b>")), edtName);
	form->addRow(new QLabel(tr("<b>Betrag:</b>")), edtCost);
	form->addRow(new QLabel(tr("<b>Datum:</b>")), edtDate);
	form->addRow(new QLabel(tr("<b>Kommentar:</b>")), edtComment);

	auto* separator = new QFrame;
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);

	auto* btnOK = new QPushButton(tr("OK"));
	auto* btnCancel = new QPushButton(tr("Cancel"));
	btnOK->setDefault(true);
	btnOK->setMinimumWidth(90);
	btnCancel->setMinimumWidth(90);

	auto* btnLayout = new QHBoxLayout;
	btnLayout->addStretch();
	btnLayout->addWidget(btnCancel);
	btnLayout->addWidget(btnOK);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(24, 20, 24, 16);
	mainLayout->setSpacing(16);
	mainLayout->addLayout(form);
	mainLayout->addStretch();
	mainLayout->addWidget(separator);
	mainLayout->addLayout(btnLayout);

	connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

dlgInputs& BalanceTabDialog::getInputs() const
{
	static dlgInputs inputs;
	inputs.name = edtName->text().toStdString();
	inputs.cost = edtCost->value();
	inputs.date = edtDate->date();
	inputs.comment = edtComment->text().toStdString();
	return inputs;
}