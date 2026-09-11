#pragma once
#include "BaseTab.h"
#include "domain/services/PaymentService.h"
#include "domain/model/Requests.h"
#include "GuiTypes.h"
#include "common/Utils.h"

class QComboBox;
class QLabel;
class QTableWidget;
class QCheckBox;
class QDoubleSpinBox;
class QPushButton;
class QRadioButton;

class PayTab : public BaseTab
{
	Q_OBJECT
	
public:
	PayTab(const LowerButtonBundle& lowerButtons, PaymentService& paymentService, 
			PersonRepository* personRepo, ConsumptionRepository* consumptionRepo, DebtRepository* debtRepo, CreditRepository* creditRepo,
			QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;
	virtual void save() override;

private:
	void nameChanged();
	void redeemCredit();
	void refreshTable(int64_t personEntryID);

	QComboBox*		nameSelect			= nullptr;
	QLabel*			totalNumLabel		= nullptr;
	QLabel*			settledNumLabel		= nullptr;
	QLabel*			dueNumLabel			= nullptr;
	QLabel*			creditNumLabel		= nullptr;
	QPushButton*	btnUseCredit		= nullptr;
	QDoubleSpinBox* paymentSpinBox		= nullptr;
	QCheckBox*		fullPaymentCheckBox = nullptr;
	QRadioButton*	btnSurplusToCredit	= nullptr;
	QRadioButton*	btnSurplusToTip		= nullptr;
	QTableWidget*	tblConsumption		= nullptr; 

	PaymentService& paymentService;
	PersonRepository* personRepo;
	ConsumptionRepository* consumptionRepo;
	DebtRepository* debtRepo;
	CreditRepository* creditRepo;
	const LowerButtonBundle& lowerButtons;

	double total{}, settled{}, due{}, credit{};

};