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
#include <QColor>


PayTab::PayTab(const LowerButtonBundle& lowerButtons, PaymentService& paymentService, PersonRepository* personRepo, QWidget* parent) 
	: lowerButtons(lowerButtons), paymentService(paymentService), personRepo(personRepo), BaseTab(parent) {}

void PayTab::initialize()
{
	nameSelect = new QComboBox(this);
	nameSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	nameSelect->setEditable(false);

	// overview
	auto* totalTextLabel	= new QLabel(tr("Gesamt"), this);
	auto* paidTextLabel		= new QLabel(tr("Beglichen"), this);
	auto* dueTextLabel		= new QLabel(tr("Ausstehend"), this);
	auto* creditTextLabel	= new QLabel(tr("Guthaben"), this);

	totalNumLabel	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	settledNumLabel	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	dueNumLabel		= new QLabel(QtUtils::toCurrencyFormat(0.0), this);
	creditNumLabel	= new QLabel(QtUtils::toCurrencyFormat(0.0), this);

	const auto configureAmountLabel = [](QLabel* label)
		{
			label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			label->setMinimumWidth(90);
		};

	configureAmountLabel(totalNumLabel);
	configureAmountLabel(settledNumLabel);
	configureAmountLabel(dueNumLabel);
	configureAmountLabel(creditNumLabel);

	QFont dueFont = dueNumLabel->font();
	dueFont.setBold(true);
	dueNumLabel->setFont(dueFont);

	btnUseCredit = new QPushButton(tr("Refresh"), this);
	btnUseCredit->setEnabled(false);

	// payment
	paymentSpinBox = new QDoubleSpinBox(this);
	paymentSpinBox->setDecimals(2);
	paymentSpinBox->setMinimum(0.0);
	paymentSpinBox->setMaximum(999999.99);
	paymentSpinBox->setSuffix(tr(" €"));
	paymentSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	fullPaymentCheckBox = new QCheckBox(tr("Ausstand übernehmen"), this);

	// surplus
	btnSurplusToCredit = new QRadioButton(tr("als Guthaben"), this);
	btnSurplusToCredit->setChecked(true);

	btnSurplusToTip = new QRadioButton(tr("als Trinkgeld"), this);

	auto* surplusGroup = new QButtonGroup(this);
	surplusGroup->addButton(btnSurplusToCredit);
	surplusGroup->addButton(btnSurplusToTip);

	// right side / table
	tblConsumption = new QTableWidget();
	tblConsumption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// left side
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
	summaryLayout->addRow(paidTextLabel, settledNumLabel);
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
			useCredit();
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
			nameChanged();
		});
}

void PayTab::nameChanged()
{
	int64_t personID = nameSelect->currentData().toLongLong();

	total = paymentService.getTotalAmount(personID);
	settled = paymentService.getSettledAmount(personID);
	due = paymentService.getDueAmount(personID);
	credit = paymentService.getCreditAmount(personID);

	// refresh table
	refreshTable(personID);

	// refresh labels
	totalNumLabel->setText(QtUtils::toCurrencyFormat(total));
	settledNumLabel->setText(QtUtils::toCurrencyFormat(settled));
	dueNumLabel->setText(QtUtils::toCurrencyFormat(due));
	creditNumLabel->setText(QtUtils::toCurrencyFormat(credit));

	// reinit checkboxes and buttons
	credit > 1e-9 ? btnUseCredit->setEnabled(true) : btnUseCredit->setEnabled(false);
	btnSurplusToCredit->setChecked(true);
	fullPaymentCheckBox->setChecked(false);
}

void PayTab::refresh()
{
	// refresh name list (clear and rebuild from database), in case it was altered
	// try to keed name loaded before
	// refresh tab for current name

	int64_t oldID = nameSelect->currentData().toLongLong();

	nameSelect->clear();
	
	std::vector<Person> personVec = personRepo->getAll();
	QList<QString> nameList = QtUtils::personVecToQStrList(personVec, &Person::getFullSpecifier);
	
	std::optional<int> indexForOldID;
	for (size_t i = 0; i < personVec.size(); i++)
	{
		int64_t itemID = personVec.at(i).getID();
		nameSelect->addItem(nameList.at(i), itemID);
		if (itemID == oldID) indexForOldID = i;
	}

	if (indexForOldID.has_value()) nameSelect->setCurrentIndex(indexForOldID.value());
			
	nameChanged();
}

void PayTab::apply()
{

}

void PayTab::save()
{

}

void PayTab::useCredit()
{

}

void PayTab::refreshTable(int64_t personID)
{
	tblConsumption->clearContents();
	
	std::vector<entry::Consumption> cEntries = paymentService.getConsumptionEntries(personID);
	std::vector<entry::DebtRemaining> drEntries = paymentService.getOutstandingEntries(personID, FilterType::IncludeFullyPaid);

	// create items and add them to table
	int rowCount = drEntries.size();
	int columnCount = 6;

	tblConsumption->setRowCount(rowCount);
	tblConsumption->setColumnCount(columnCount);
	tblConsumption->setHorizontalHeaderLabels(QtUtils::strVecToQStrList({ "Monat", "Bezahlt/Gesamt (€)", "Bier (0.5l)", "Bier (0.4l)", "Wasser", "Softdrinks"}));

	QColor rowColor;
	for (size_t row = 0; row < drEntries.size(); row++)
	{
		const entry::DebtRemaining& drEntry = drEntries.at(row);

		if (drEntry.remaining > 1e-9)
			rowColor = QColor(Qt::GlobalColor::red);
		else
			rowColor = QColor(Qt::GlobalColor::green);

		// find corresponding consumption info, if available
		std::optional<entry::Consumption> cEntryMatching;
		for (auto& cEntry : cEntries)
		{
			if (cEntry.debtEntryID = drEntry.debtEntryID)
			{
				cEntryMatching = cEntry;
				break;
			}
		}

		QTableWidgetItem* dateItem = new QTableWidgetItem(QtUtils::extractMonth(drEntry.date));
		QTableWidgetItem* infoItem = new QTableWidgetItem(QString::number(drEntry.amount-drEntry.remaining, 'f', 2) + QString::fromStdString("/") + QString::number(drEntry.amount, 'f', 2));

		QTableWidgetItem* beer05Item;
		QTableWidgetItem* beer04Item;
		QTableWidgetItem* softdrinksItem;
		QTableWidgetItem* waterItem;

		if (cEntryMatching.has_value())
		{
			beer05Item		= new QTableWidgetItem(QString::number(cEntryMatching.value().nBeer05));
			beer04Item		= new QTableWidgetItem(QString::number(cEntryMatching.value().nBeer04));
			softdrinksItem	= new QTableWidgetItem(QString::number(cEntryMatching.value().nSoftdrinks));
			waterItem		= new QTableWidgetItem(QString::number(cEntryMatching.value().nWater));
		}
		else
		{
			auto content = QString::fromStdString("unb.");

			beer05Item		= new QTableWidgetItem(content);
			beer04Item		= new QTableWidgetItem(content);
			softdrinksItem	= new QTableWidgetItem(content);
			waterItem		= new QTableWidgetItem(content);
		}

		tblConsumption->setItem(row, 0, dateItem);
		tblConsumption->setItem(row, 1, infoItem);
		tblConsumption->setItem(row, 2, beer05Item);
		tblConsumption->setItem(row, 3, beer04Item);
		tblConsumption->setItem(row, 4, softdrinksItem);
		tblConsumption->setItem(row, 5, waterItem);

		//for (int col = 0; col < columnCount; col++) tblConsumption->item(row, col)->setBackground(rowColor);
	}
}