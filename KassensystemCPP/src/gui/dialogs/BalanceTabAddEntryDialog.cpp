#include "gui/dialogs/BalanceTabAddEntryDialog.h"
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
#include <algorithm>

BalanceTabAddEntryDialog::BalanceTabAddEntryDialog(BtnIndex mode, std::vector<entry::Person>& personVec, QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Eintrag hinzufügen");
	
	switch (mode)
	{
	case BtnIndex::addEarning:
		setWindowTitle(QStringLiteral("Einnahme hinzufügen"));
		break;
	case BtnIndex::addSpending:
		setWindowTitle(QStringLiteral("Ausgabe hinzufügen"));
		break;
	}

	QFont boldFont = font();
	boldFont.setBold(true);

	edtDescription = new QLineEdit();
	edtDescription->setPlaceholderText(QStringLiteral("z. B. Spende, Geschenk, ..."));

	edtCost = new QDoubleSpinBox();
	edtCost->setRange(0.0, 1'000'000.0);
	edtCost->setDecimals(2);
	edtCost->setSuffix(QStringLiteral(" ") + qtUtils::eurSymbol());
	edtCost->setSingleStep(1.0);
	edtCost->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	edtDate = new QDateEdit(QDate::currentDate());
	edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	edtDate->setCalendarPopup(true);

	auto* vLine = new QFrame(this);
	vLine->setFrameShape(QFrame::VLine);
	vLine->setFrameShadow(QFrame::Sunken);

	edtIsSpecial = new QCheckBox("Sonstiger Zeitraum:", this);
	edtIsSpecial->setChecked(false);

	std::map<QString, RegisterDate::Special> specialMap =
	{
		{"Vergangenheit", RegisterDate::Special::previous},
		{"Unbekannt", RegisterDate::Special::unknown},
		{"Zukunft", RegisterDate::Special::subsequent}
	};
	edtSpecial = new QComboBox(this);
	for (const auto& [specialStr, specialData] : specialMap)
	{
		edtSpecial->insertItem(0, specialStr, QVariant(static_cast<int>(specialData)));
	}
	edtSpecial->setCurrentIndex(edtSpecial->findText("Unbekannt"));
	edtSpecial->setEnabled(false);

	auto* dateLayout = new QHBoxLayout();
	dateLayout->addWidget(edtDate, 3);
	dateLayout->addWidget(vLine);
	dateLayout->addWidget(edtIsSpecial, 1, Qt::AlignRight);
	dateLayout->addWidget(edtSpecial, 1);


	edtComment = new QPlainTextEdit();
	edtComment->setPlaceholderText(QStringLiteral("optional"));
	edtComment->setFixedHeight(70);

	auto* lblDescription = new QLabel(QStringLiteral("Bezeichnung:"));
	lblDescription->setFont(boldFont);
	auto* lblCost = new QLabel(QStringLiteral("Betrag:"));
	lblCost->setFont(boldFont);
	auto* lblDate = new QLabel(QStringLiteral("Datum:"));
	lblDate->setFont(boldFont);
	auto* lblComment = new QLabel(QStringLiteral("Kommentar:"));
	lblComment->setFont(boldFont);

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblDescription, edtDescription);
	form->addRow(lblCost, edtCost);
	form->addRow(lblDate, dateLayout);
	form->addRow(lblComment, edtComment);


	edtIsCovered = new QCheckBox(QStringLiteral("Von Mitglied getragen:"));
	edtIsCovered->setChecked(false);

	edtCoveringPerson = new QComboBox();
	edtCoveringPerson->setEnabled(false);
	std::sort(personVec.begin(), personVec.end(), [](const entry::Person& a, const entry::Person& b)
		{
			return a.getFullSpecifier() > b.getFullSpecifier();
		});
	QList<QString> nameList = qtUtils::personVecToQStrList(personVec, &entry::Person::getFullName);
	for (size_t i = personVec.size(); i-- > 0; )
		edtCoveringPerson->addItem(
			nameList.at(i),
			QVariant::fromValue(personVec.at(i))
		);

	auto* statusLayout = new QHBoxLayout;
	statusLayout->addWidget(edtIsCovered);
	statusLayout->addWidget(edtCoveringPerson);
	statusLayout->addStretch();

	auto* sectionLabel = new QLabel(QStringLiteral("Zahlungsstatus - wird als Guthaben gutgeschrieben"));
	sectionLabel->setFont(boldFont);

	auto* topSeparator = new QFrame;
	topSeparator->setFrameShape(QFrame::HLine);
	topSeparator->setFrameShadow(QFrame::Sunken);

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
	if (mode == BtnIndex::addSpending)
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
	connect(edtIsSpecial, &QCheckBox::checkStateChanged, this, [&](Qt::CheckState state) {
		switch (state)
		{
		case Qt::Checked:
			edtDate->setEnabled(false);
			edtSpecial->setEnabled(true);
			break;
		case Qt::Unchecked:
			edtDate->setEnabled(true);
			edtSpecial->setEnabled(false);
			break;
		}
		});

	setMinimumWidth(520);
	adjustSize();

	edtDescription->setFocus(Qt::TabFocusReason);
}

BalanceTabAddEntryDialog::inputs BalanceTabAddEntryDialog::getInputs() const
{
	return BalanceTabAddEntryDialog::inputs{
		.description = edtDescription->text().toStdString(),
		.amount = edtCost->value(),
		.date = edtIsSpecial->isChecked() ? 
			RegisterDate{static_cast<RegisterDate::Special>(edtSpecial->currentData().toInt())} :
			RegisterDate{edtDate->date()},
		.comment = edtComment->toPlainText().toStdString(),
		.coveringPerson = edtIsCovered->isChecked() ? 
			std::optional<entry::Person>(edtCoveringPerson->currentData().value<entry::Person>()) : 
			std::optional<entry::Person>(std::nullopt)
			// std::optional-casting needed as ternary operator expects same datatypes in both branches
	};
}