#include "PayTabAddPersonDialog.h"
#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QFont>
#include <string>

PayTabAddPersonDialog::PayTabAddPersonDialog(QWidget* parent) : QDialog(parent)
{
	QFont boldFont = font();
	boldFont.setBold(true);

	edtFirstName	= new QLineEdit();
	edtLastName		= new QLineEdit();
	edtNickName		= new QLineEdit();
	edtInfo			= new QLineEdit();

	auto* lblFirstName = new QLabel(QStringLiteral("Vor-/Gruppenname:"));
	auto* lblLastName = new QLabel(QStringLiteral("Nachname:"));
	auto* lblNickName = new QLabel(QStringLiteral("Spitzname:"));
	auto* lblInfo = new QLabel(QStringLiteral("Info:"));
	
	lblFirstName->setFont(boldFont);
	lblLastName->setFont(boldFont);
	lblNickName->setFont(boldFont);
	lblInfo->setFont(boldFont);

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblFirstName, edtFirstName);
	form->addRow(lblLastName, edtLastName);
	form->addRow(lblNickName, edtNickName);
	form->addRow(lblInfo, edtInfo);
	
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

PayTabAddPersonDialog::inputs PayTabAddPersonDialog::getInputs() const
{
	return PayTabAddPersonDialog::inputs{
		.firstName = edtFirstName->text().toStdString(),
		.lastName = edtLastName->text().toStdString(),
		.nickName = edtNickName->text().toStdString(),
		.info = edtInfo->text().toStdString()
	};
}