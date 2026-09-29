#include "BalanceTabSettlementDialog.h"
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

	//edtDate = new QDateEdit(QDate::currentDate());
	//edtDate->setDisplayFormat(QStringLiteral("dd MMMM yy"));
	//edtDate->setCalendarPopup(true);

	//auto* vLine = new QFrame(this);
	//vLine->setFrameShape(QFrame::VLine);
	//vLine->setFrameShadow(QFrame::Sunken);

	//edtIsSpecial = new QCheckBox("Sonstiger Zeitraum:", this);
	//edtIsSpecial->setChecked(false);

	//std::map<QString, RegisterDate::Special> specialMap =
	//{
	//	{"Vergangenheit", RegisterDate::Special::previous},
	//	{"Unbekannt", RegisterDate::Special::unknown},
	//	{"Zukunft", RegisterDate::Special::subsequent}
	//};
	//edtSpecial = new QComboBox(this);
	//for (const auto& [specialStr, specialData] : specialMap)
	//{
	//	edtSpecial->insertItem(0, specialStr, QVariant(static_cast<int>(specialData)));
	//}
	//edtSpecial->setCurrentIndex(edtSpecial->findText("Unbekannt"));
	//edtSpecial->setEnabled(false);

	//auto* dateLayout = new QHBoxLayout();
	//dateLayout->addWidget(edtDate, 3);
	//dateLayout->addWidget(vLine);
	//dateLayout->addWidget(edtIsSpecial, 1, Qt::AlignRight);
	//dateLayout->addWidget(edtSpecial, 1);


	edtComment = new QPlainTextEdit();
	edtComment->setPlaceholderText(QStringLiteral("optional"));
	edtComment->setFixedHeight(70);

	auto* lblAmount = new QLabel(QStringLiteral("Betrag:"));
	lblAmount->setFont(boldFont);
	//auto* lblDate = new QLabel(QStringLiteral("Datum:"));
	//lblDate->setFont(boldFont);
	auto* lblComment = new QLabel(QStringLiteral("Kommentar:"));
	lblComment->setFont(boldFont);

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignLeft);
	form->setFormAlignment(Qt::AlignTop);
	form->setHorizontalSpacing(16);
	form->setVerticalSpacing(10);
	form->setRowWrapPolicy(QFormLayout::WrapAllRows);

	form->addRow(lblAmount, edtAmount);
	//form->addRow(lblDate, dateLayout);
	form->addRow(lblComment, edtComment);

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
	//connect(edtIsSpecial, &QCheckBox::checkStateChanged, this, [&](Qt::CheckState state) {
	//	switch (state)
	//	{
	//	case Qt::Checked:
	//		edtDate->setEnabled(false);
	//		edtSpecial->setEnabled(true);
	//		break;
	//	case Qt::Unchecked:
	//		edtDate->setEnabled(true);
	//		edtSpecial->setEnabled(false);
	//		break;
	//	}
	//	});

	setMinimumWidth(520);
	adjustSize();
}

BalanceTabSettlementDialog::inputs BalanceTabSettlementDialog::getInputs() const
{
	return BalanceTabSettlementDialog::inputs{
		.amount = edtAmount->value(),
		.comment = edtComment->toPlainText().toStdString()
	};
}