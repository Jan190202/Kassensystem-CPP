#include "BalanceTabDialog.h"
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
#include <QCheckBox>
#include <QComboBox>
#include <QFont>
#include <QPlainTextEdit>

#include <string>

BalanceTabDialog::BalanceTabDialog(btnIndex mode, std::vector<std::string> personNames, QWidget* parent) : QDialog(parent)
{
	switch (mode)
	{
	case btnIndex::addEarning:
		setWindowTitle(tr("Einnahme hinzufügen"));
		break;
	case btnIndex::addSpending:
		setWindowTitle(tr("Ausgabe hinzufügen"));
		break;
	}

	// --- zentral definierter Fett-Font für alle Sektions-/Feld-Labels ---
	QFont boldFont = font();
	boldFont.setBold(true);

	// --- Eingabe-Widgets: Hauptdaten ---
	edtDescription = new QLineEdit();
	edtDescription->setPlaceholderText(tr("z. B. Spende, Geschenk, ..."));

	edtCost = new QDoubleSpinBox();
	edtCost->setRange(0.0, 1'000'000.0);
	edtCost->setDecimals(2);
	edtCost->setSuffix(QStringLiteral(" €"));
	edtCost->setSingleStep(1.0);
	edtCost->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	edtDate = new QDateEdit(QDate::currentDate());
	edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	edtDate->setCalendarPopup(true);

	edtComment = new QPlainTextEdit();
	edtComment->setPlaceholderText(tr("optional"));
	edtComment->setFixedHeight(70);

	// --- Sektion 1: Formular für die Basisdaten ---
	auto* lblDescription = new QLabel(tr("Bezeichnung:"));
	lblDescription->setFont(boldFont);
	auto* lblCost = new QLabel(tr("Betrag:"));
	lblCost->setFont(boldFont);
	auto* lblDate = new QLabel(tr("Datum:"));
	lblDate->setFont(boldFont);
	auto* lblComment = new QLabel(tr("Kommentar:"));
	lblComment->setFont(boldFont);

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblDescription, edtDescription);
	form->addRow(lblCost, edtCost);
	form->addRow(lblDate, edtDate);
	form->addRow(lblComment, edtComment);

	// --- Sektion 2: Zahlungsstatus (visuell abgetrennt) ---
	edtIsCovered = new QCheckBox(tr("Von Mitglied getragen:"));
	edtIsCovered->setChecked(false);

	edtCoveringPerson = new QComboBox();
	edtCoveringPerson->setEnabled(false);
	edtCoveringPerson->insertItems(0, QtUtils::strVecToQStrList(personNames));

	// Checkbox + Dropdown eng nebeneinander statt über volle Formularbreite verteilt
	auto* statusLayout = new QHBoxLayout;
	statusLayout->addWidget(edtIsCovered);
	statusLayout->addWidget(edtCoveringPerson);
	statusLayout->addStretch();

	auto* sectionLabel = new QLabel(tr("Zahlungsstatus - wird als Guthaben gutgeschrieben"));
	sectionLabel->setFont(boldFont);

	auto* topSeparator = new QFrame;
	topSeparator->setFrameShape(QFrame::HLine);
	topSeparator->setFrameShadow(QFrame::Sunken);

	// --- Trennlinie vor den Buttons ---
	auto* bottomSeparator = new QFrame;
	bottomSeparator->setFrameShape(QFrame::HLine);
	bottomSeparator->setFrameShadow(QFrame::Sunken);

	// --- Buttons, rechtsbündig ---
	auto* btnOK = new QPushButton(tr("OK"));
	auto* btnCancel = new QPushButton(tr("Cancel"));
	btnOK->setDefault(true);
	btnOK->setMinimumWidth(90);
	btnCancel->setMinimumWidth(90);

	auto* btnLayout = new QHBoxLayout;
	btnLayout->addStretch();
	btnLayout->addWidget(btnCancel);
	btnLayout->addWidget(btnOK);

	// --- Gesamtlayout ---
	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(24, 20, 24, 16);
	mainLayout->setSpacing(12);

	mainLayout->addLayout(form);
	mainLayout->addSpacing(8);
	if (mode == btnIndex::addSpending)
	{
		mainLayout->addWidget(topSeparator);
		mainLayout->addWidget(sectionLabel);
		mainLayout->addLayout(statusLayout);
	}
	mainLayout->addStretch();
	mainLayout->addWidget(bottomSeparator);
	mainLayout->addLayout(btnLayout);

	connect(edtIsCovered, &QCheckBox::toggled, edtCoveringPerson, &QComboBox::setEnabled);

	connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

	// Mindestbreite vorgeben, damit adjustSize() nicht zu schmal berechnet
	setMinimumWidth(520);
	adjustSize();
}

dlgInputs& BalanceTabDialog::getInputs() const
{
	static dlgInputs inputs;
	inputs.description = edtDescription->text().toStdString();
	inputs.cost = edtCost->value();
	inputs.date = edtDate->date();
	inputs.comment = edtComment->toPlainText().toStdString();
	inputs.isCovered = edtIsCovered->isChecked();
	if (inputs.isCovered)
	{
		inputs.coveringPerson = edtCoveringPerson->currentText().toStdString();
	}
	return inputs;
}