#pragma once
#include "BaseTab.h"
#include <vector>

class AddTab : public BaseTab
{
	Q_OBJECT
public:
	AddTab(QWidget* parent = nullptr);
	virtual void initialize() override;
private:
	//std::vector<AddTabEntry*> entries;
	//void addEntry();
	//void removeEntry(AddTabEntry* entry);
	//void clearEntries();
};