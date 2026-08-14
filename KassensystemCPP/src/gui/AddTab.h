#pragma once

#include "BaseTab.h"

#include <vector>

class AddTabEntry;
class QDateEdit;
class QGridLayout;
class QPushButton;
class QVBoxLayout;

class AddTab : public BaseTab
{
	Q_OBJECT

public:
	AddTab(QWidget* parent = nullptr);
	void initialize() override;

private slots:
	void addEntry();

private:
	QDateEdit* monthSelection = nullptr;
	std::vector<AddTabEntry*> entries;

	QPushButton* btnAddEntry = nullptr;
	QVBoxLayout* addTabMainLayout = nullptr;
	QGridLayout* entriesGrid = nullptr;

	void removeEntry(AddTabEntry* entry);
	void clearEntries();
	void shiftEntries();
};