#include "PayTab.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSizePolicy>
#include <QTableWidget>
#include <QVBoxLayout>

PayTab::PayTab(QWidget* parent) : BaseTab(parent) {}

void PayTab::initialize()
{
	nameSelect = new QComboBox(this);
	nameSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// Betragsübersicht
	auto* totalTextLabel	= new QLabel(tr("Gesamt"), this);
	auto* paidTextLabel		= new QLabel(tr("Bezahlt"), this);
	auto* dueTextLabel		= new QLabel(tr("Ausstehend"), this);
	auto* creditTextLabel	= new QLabel(tr("Guthaben"), this);

	totalNumLabel	= new QLabel(tr("0,00 €"), this);
	paidNumLabel	= new QLabel(tr("0,00 €"), this);
	dueNumLabel		= new QLabel(tr("0,00 €"), this);
	creditNumLabel	= new QLabel(tr("0,00 €"), this);

	const auto configureAmountLabel = [](QLabel* label)
		{
			label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			label->setMinimumWidth(90);
		};

	configureAmountLabel(totalNumLabel);
	configureAmountLabel(paidNumLabel);
	configureAmountLabel(dueNumLabel);
	configureAmountLabel(creditNumLabel);

	QFont dueFont = dueNumLabel->font();
	dueFont.setBold(true);
	dueNumLabel->setFont(dueFont);

	btnUseCredit = new QPushButton(tr("Refresh"), this);
	btnUseCredit->setEnabled(false);

	// Zahlung
	paymentSpinBox = new QDoubleSpinBox(this);
	paymentSpinBox->setDecimals(2);
	paymentSpinBox->setMinimum(0.0);
	paymentSpinBox->setMaximum(999999.99);
	paymentSpinBox->setSuffix(tr(" €"));
	paymentSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	fullPaymentCheckBox = new QCheckBox(tr("Ausstand übernehmen"), this);

	// Überschuss
	btnSurplusToCredit = new QRadioButton(tr("als Guthaben"), this);
	btnSurplusToCredit->setChecked(true);

	btnSurplusToTip = new QRadioButton(tr("als Trinkgeld"), this);

	auto* surplusGroup = new QButtonGroup(this);
	surplusGroup->addButton(btnSurplusToCredit);
	surplusGroup->addButton(btnSurplusToTip);

	// Rechte Seite / Tabelle
	tblConsumption = new QTableView();
	tblConsumption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Linke Seite
	auto* leftLayout = new QVBoxLayout();
	leftLayout->setContentsMargins(0, 0, 0, 0);
	leftLayout->setSpacing(14);

	auto* customerBox = new QGroupBox(tr("Person"), this);
	auto* customerLayout = new QVBoxLayout(customerBox);
	customerLayout->setContentsMargins(12, 14, 12, 12);
	customerLayout->addWidget(nameSelect);

	auto* summaryBox = new QGroupBox(tr("Übersicht"), this);
	auto* summaryLayout = new QFormLayout(summaryBox);
	summaryLayout->setContentsMargins(12, 14, 12, 12);
	summaryLayout->setHorizontalSpacing(16);
	summaryLayout->setVerticalSpacing(9);
	summaryLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	summaryLayout->addRow(totalTextLabel, totalNumLabel);
	summaryLayout->addRow(paidTextLabel, paidNumLabel);
	summaryLayout->addRow(dueTextLabel, dueNumLabel);

	auto* creditLayout = new QHBoxLayout();
	creditLayout->setContentsMargins(0, 0, 0, 0);
	creditLayout->setSpacing(8);
	creditLayout->addWidget(creditNumLabel, 1);
	creditLayout->addWidget(btnUseCredit);

	summaryLayout->addRow(creditTextLabel, creditLayout);

	auto* paymentBox = new QGroupBox(tr("Zahlung erfassen"), this);
	auto* paymentLayout = new QVBoxLayout(paymentBox);
	paymentLayout->setContentsMargins(12, 14, 12, 12);
	paymentLayout->setSpacing(10);

	auto* amountLayout = new QFormLayout();
	amountLayout->setContentsMargins(0, 0, 0, 0);
	amountLayout->setHorizontalSpacing(16);
	amountLayout->addRow(tr("Betrag"), paymentSpinBox);

	paymentLayout->addLayout(amountLayout);
	paymentLayout->addWidget(fullPaymentCheckBox);

	auto* surplusBox = new QGroupBox(tr("Überschuss behandeln"), this);
	auto* surplusLayout = new QHBoxLayout(surplusBox);
	surplusLayout->setContentsMargins(12, 14, 12, 12);
	surplusLayout->setSpacing(18);
	surplusLayout->addWidget(btnSurplusToCredit);
	surplusLayout->addWidget(btnSurplusToTip);
	surplusLayout->addStretch();

	leftLayout->addWidget(customerBox);
	leftLayout->addWidget(summaryBox);
	leftLayout->addWidget(paymentBox);
	leftLayout->addWidget(surplusBox);
	leftLayout->addStretch();

	auto* vLine = new QFrame(this);
	vLine->setFrameShape(QFrame::VLine);
	vLine->setFrameShadow(QFrame::Sunken);

	auto* mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(12, 12, 12, 12);
	mainLayout->setSpacing(18);

	mainLayout->addLayout(leftLayout, 1);
	mainLayout->addWidget(vLine);
	mainLayout->addWidget(tblConsumption, 3);
}