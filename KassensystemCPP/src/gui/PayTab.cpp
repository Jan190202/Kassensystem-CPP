#include "PayTab.h"
#include "qtutils/QtConversions.h"

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


PayTab::PayTab(const LowerButtonBundle& lowerButtons, PaymentService& paymentService, PersonRepository* personRepo, QWidget* parent) 
	: lowerButtons(lowerButtons), paymentService(paymentService), personRepo(personRepo), BaseTab(parent) {}

void PayTab::initialize()
{
	nameSelect = new QComboBox(this);
	nameSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	nameSelect->setEditable(false);

	// Betragsübersicht
	auto* totalTextLabel	= new QLabel(tr("Gesamt"), this);
	auto* paidTextLabel		= new QLabel(tr("Bezahlt"), this);
	auto* dueTextLabel		= new QLabel(tr("Ausstehend"), this);
	auto* creditTextLabel	= new QLabel(tr("Guthaben"), this);

	totalNumLabel	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	paidNumLabel	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	dueNumLabel		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	creditNumLabel	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);

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
	tblConsumption = new QTableWidget();
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

	refresh();

	connect(lowerButtons.btnApply, &QPushButton::clicked, this, [&]()
		{
			PaymentRequest request{
				.personID = nameSelect->currentData().toLongLong(),
				.date = QDate::currentDate(),
				.amount = paymentSpinBox->value(),
				.overpaymentType = btnSurplusToCredit->isChecked() ? OverpaymentDisposition::Credit : OverpaymentDisposition::Tip
			};

			paymentService.addPayment(request);
		});

	connect(btnUseCredit, &QPushButton::clicked, this, [&]()
		{
			// TBD
		});

	connect(fullPaymentCheckBox, &QCheckBox::checkStateChanged, this, [&](Qt::CheckState state)
		{
			switch (state)
			{
			case Qt::Checked:
				paymentSpinBox->setEnabled(false);
				paymentSpinBox->setValue(due);
				break;
			case Qt::Unchecked:
				paymentSpinBox->setEnabled(true);
				paymentSpinBox->setValue(0.0);
				break;
			}
		});

	connect(nameSelect, &QComboBox::currentTextChanged, this, [&]()
		{
			std::string currentName = nameSelect->currentText().toStdString();
			// TBD
		});
}

void PayTab::nameChanged()
{
	int64_t personID = nameSelect->currentData().toLongLong();

	total = paymentService.getTotalAmount(personID);
	paid = paymentService.getPaidAmount(personID);
	credit = paymentService.getCreditAmount(personID);
	due = total - paid;

	// refresh table
	// TBD

	// refresh labels
	totalNumLabel->setText(QtUtils::toCurrencyFormat(total));
	paidNumLabel->setText(QtUtils::toCurrencyFormat(paid));
	dueNumLabel->setText(QtUtils::toCurrencyFormat(due));
	creditNumLabel->setText(QtUtils::toCurrencyFormat(credit));

	// reinit checkboxes and buttons
	credit > 1e-9 ? btnUseCredit->setEnabled(true) : btnUseCredit->setEnabled(false);
	btnSurplusToCredit->setChecked(true);
	fullPaymentCheckBox->setChecked(false);
}

void PayTab::refresh()
{
	nameSelect->clear();
	
	std::vector<Person> personVec = personRepo->getAll();
	QList<QString> nameList = QtUtils::personVecToQStrList(personVec, &Person::getFullSpecifier);
	for (size_t i = 0; i < personVec.size(); i++)
		nameSelect->addItem(
			nameList.at(i),
			personVec.at(i).getID()
		);

	nameChanged();
}
