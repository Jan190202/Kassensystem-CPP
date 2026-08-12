#pragma once
#include "BaseTab.h"

class BalanceTab : public BaseTab
{
	Q_OBJECT
public:
	BalanceTab(QWidget* parent = nullptr);
	virtual void initialize() override;
private:

};