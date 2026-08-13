#pragma once
#include "BaseTab.h"
#include <vector>
class AddTabEntry;
class QDateEdit;
class QPushButton;
class QVBoxLayout;
class QGridLayout;

class AddTab : public BaseTab
{
	Q_OBJECT
public:
	AddTab(QWidget* parent = nullptr);
	virtual void initialize() override;
private slots:
	void addEntry();
private:
	QDateEdit* monthSelection = nullptr;
	std::vector<AddTabEntry*> entries = {};
	QPushButton* btnAddEntry = nullptr;
	QVBoxLayout* AddTabMainLayout = nullptr;
	QGridLayout* entriesGrid = nullptr;

	void removeEntry(AddTabEntry* entry);
	void clearEntries();
};