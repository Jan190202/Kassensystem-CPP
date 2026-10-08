#pragma once
#include "gui/interfaces/BaseTab.h"
#include "gui/types/GuiTypes.h"
#include "domain/services/PaymentService.h"
#include "domain/services/PersonService.h"
#include "domain/model/Requests.h"
#include "common/Utils.h"

class QComboBox;
class QLabel;
class QTableWidget;
class QCheckBox;
class QDoubleSpinBox;
class QPushButton;
class QRadioButton;
class QPlainTextEdit;

class PayTab : public BaseTab
{
	Q_OBJECT
	
public:
	PayTab(PaymentService& paymentService, PersonService& personService, 
			PersonRepository* personRepo, ConsumptionRepository* consumptionRepo, DebtRepository* debtRepo, CreditRepository* creditRepo,
			QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private:
	void nameChanged();
	void redeemCredit(const entry::Person& person);
	void allRedeemCredit();
	void addCredit();
	void addPerson();
	void refreshTable(int64_t personEntryID);
	void exportData();

	QComboBox*		nameSelect			= nullptr;
	QPushButton*	btnAddPerson		= nullptr;
	QPushButton*	btnExport			= nullptr;
	QLabel*			totalNumLabel		= nullptr;
	QLabel*			settledNumLabel		= nullptr;
	QLabel*			dueNumLabel			= nullptr;
	QLabel*			creditNumLabel		= nullptr;
	QPushButton*	btnUseCredit		= nullptr;
	QPushButton*	btnAllUseCredit		= nullptr;
	QPushButton*	btnAddCredit		= nullptr;
	QDoubleSpinBox* paymentSpinBox		= nullptr;
	QCheckBox*		fullPaymentCheckBox = nullptr;
	QRadioButton*	btnSurplusToCredit	= nullptr;
	QRadioButton*	btnSurplusToTip		= nullptr;
	QTableWidget*	tblConsumption		= nullptr; 
	QPlainTextEdit* edtComment			= nullptr;

	PaymentService& paymentService;
	PersonService& personService;
	PersonRepository* personRepo;
	ConsumptionRepository* consumptionRepo;
	DebtRepository* debtRepo;
	CreditRepository* creditRepo;

	double total{}, settled{}, due{}, credit{};

};