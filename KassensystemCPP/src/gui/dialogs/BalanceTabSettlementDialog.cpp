#include "gui/dialogs/BalanceTabSettlementDialog.h"
#include "qtutils/QtConversions.h"
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

BalanceTabSettlementDialog::BalanceTabSettlementDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Fremdanteil begleichen");

	QFont boldFont = font();
	boldFont.setBold(true);

	edtAmount = new QDoubleSpinBox();
	edtAmount->setRange(0.0, 1'000'000.0);
	edtAmount->setDecimals(2);
	edtAmount->setSuffix(QStringLiteral(" ") + qtUtils::eurSymbol());
	edtAmount->setSingleStep(1.0);
	edtAmount->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	edtDate = new QDateEdit(QDate::currentDate());
	edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	edtDate->setCalendarPopup(true);
	edtDate->setEnabled(false);

	edtIsAdvanced = new QCheckBox();
	edtIsAdvanced->setText("erweiterte Einstellungen");
	edtIsAdvanced->setChecked(false);

	edtComment = new QPlainTextEdit();
	edtComment->setPlaceholderText(QStringLiteral("optional"));
	edtComment->setFixedHeight(70);

	auto* lblAmount = new QLabel(QStringLiteral("Betrag:"));
	lblAmount->setFont(boldFont);
	auto* lblComment = new QLabel(QStringLiteral("Kommentar:"));
	lblComment->setFont(boldFont);
	auto* lblDate = new QLabel(QStringLiteral("Datum (erweitert, kein Effekt auf Allokation):"));
	lblDate->setFont(boldFont);

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblAmount, edtAmount);
	form->addRow(lblComment, edtComment);
	form->addRow(lblDate, edtDate);

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
	mainLayout->addWidget(edtIsAdvanced);
	mainLayout->addSpacing(8);
	mainLayout->addStretch();
	mainLayout->addWidget(bottomSeparator);
	mainLayout->addLayout(btnLayout);

	connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
	connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
	connect(edtIsAdvanced, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state)
		{
			switch (state)
			{
			case Qt::Checked:
				edtAmount->setDecimals(3);
				edtDate->setEnabled(true);
				break;
			case Qt::Unchecked:
				edtAmount->setDecimals(2);
				edtDate->setEnabled(false);
				break;
			}
		});

	setMinimumWidth(520);
	adjustSize();
}

BalanceTabSettlementDialog::inputs BalanceTabSettlementDialog::getInputs() const
{
	return BalanceTabSettlementDialog::inputs{
		.amount = edtAmount->value(),
		.comment = edtComment->toPlainText().toStdString(),
		.dateAddedAdvanced =
			edtIsAdvanced->isChecked() ?
				std::optional<QDate>(edtDate->date()) :
				std::optional<QDate>(std::nullopt)
	};
}