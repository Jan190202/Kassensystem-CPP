#pragma once
#include "BaseTab.h"
#include "domain/services/PaymentService.h"
#include "domain/model/Requests.h"
#include "GuiTypes.h"

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
	PayTab(const LowerButtonBundle& lowerButtons, PaymentService& paymentService, QWidget* parent = nullptr);
	virtual void initialize() override;
private:
	QComboBox*		nameSelect			= nullptr;
	QLabel*			totalNumLabel		= nullptr;
	QLabel*			paidNumLabel		= nullptr;
	QLabel*			dueNumLabel			= nullptr;
	QLabel*			creditNumLabel		= nullptr;
	QPushButton*	btnUseCredit		= nullptr;
	QDoubleSpinBox* paymentSpinBox		= nullptr;
	QCheckBox*		fullPaymentCheckBox = nullptr;
	QRadioButton*	btnSurplusToCredit	= nullptr;
	QRadioButton*	btnSurplusToTip		= nullptr;
	QTableWidget*	tblConsumption		= nullptr; 

	PaymentService& paymentService;
	const LowerButtonBundle& lowerButtons;
private slots:
	void personChanged(int64_t personID);
};