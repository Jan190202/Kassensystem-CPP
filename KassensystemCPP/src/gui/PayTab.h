#pragma once
#include "BaseTab.h"

class QCombobox;


class PayTab : public BaseTab
{
	Q_OBJECT
public:
	PayTab(QWidget* parent = nullptr);
	virtual void initialize() override;
private:
	QCombobox* nameSelect = nullptr;
};