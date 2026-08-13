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
	QDateEdit* monthSelection;
	std::vector<AddTabEntry*> entries;
	QPushButton* btnAddEntry;
	QVBoxLayout* AddTabMainLayout;
	QGridLayout* entriesGrid;

	void removeEntry(AddTabEntry* entry);
	void clearEntries();
};