#pragma once
#include "BaseTab.h"
#include <vector>
#include "AddTabEntry.h"

class AddTab : public BaseTab
{
	Q_OBJECT
public:
	AddTab(QWidget* parent = nullptr);
	void initialize() override;
private:
	std::vector<AddTabEntry*> entries;
};