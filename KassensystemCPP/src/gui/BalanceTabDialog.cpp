#include "BalanceTabDialog.h"

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QDate>

#include <string>

BalanceTabDialog::BalanceTabDialog(const std::string& dlgName, QWidget* parent) : QDialog(parent)
{
	QString dlgNameQ = QString::fromStdString(dlgName);

	open();
	resize(400, 500);
	setWindowTitle(dlgNameQ);
	
	
	// input dialogs: entry name, cost, date, comment
	auto* edtName		= new QLineEdit();
	auto* edtCost		= new QDoubleSpinBox();
	auto* edtDate		= new QDateEdit(QDate::currentDate());
	edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	edtDate->setCalendarPopup(true);

	auto* edtComment	= new QLineEdit();

	auto* btnOK		= new QPushButton(tr("OK"));
	auto* btnCancel = new QPushButton(tr("Cancel"));

	

	auto* mainLayout = new QVBoxLayout(this);
	auto* btnLayout = new QHBoxLayout;
	
	mainLayout->addWidget(new QLabel("Bezeichnung:"));
	mainLayout->addWidget(edtName);
	mainLayout->addWidget(new QLabel("Betrag:"));
	mainLayout->addWidget(edtCost);
	mainLayout->addWidget(new QLabel("Datum:"));
	mainLayout->addWidget(edtDate);
	mainLayout->addWidget(new QLabel("Kommentar:"));
	mainLayout->addWidget(edtComment);

	mainLayout->addLayout(btnLayout);
	

	btnLayout->addWidget(btnCancel);
	btnLayout->addWidget(btnOK);
	
	connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}