#pragma once
#include "BaseTab.h"

class BalanceTab : public BaseTab
{
	Q_OBJECT
public:
	BalanceTab(QWidget* parent = nullptr);
	void initialize() override;
private:

};