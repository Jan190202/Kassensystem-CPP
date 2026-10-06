#pragma once
#include "gui/interfaces/BaseTab.h"
#include "gui/types/GuiTypes.h"
#include "domain/services/ConsumptionService.h"
#include <vector>
#include <variant>

class AddTabEntry;
class QDateEdit;
class QGridLayout;
class QPushButton;
class QComboBox;
class QCheckBox;
class QVBoxLayout;

class AddTab : public BaseTab
{
	Q_OBJECT
public:
	AddTab(ConsumptionService& consumptionService, PersonRepository* personRepo, QWidget* parent = nullptr);
	virtual void initialize() override;
	virtual void refresh() override;
	virtual void apply() override;

private slots:
	void addEntry();

private:
	QDateEdit* monthSelection = nullptr;
	QCheckBox* specialDateCheck = nullptr;
	QComboBox* specialDateSelection = nullptr;
	std::vector<AddTabEntry*> entries;

	QPushButton* btnAddEntry = nullptr;
	QVBoxLayout* addTabMainLayout = nullptr;
	QGridLayout* entriesGrid = nullptr;

	void removeEntry(AddTabEntry* entry);
	void clearEntries();
	void shiftEntries();
	void handleInputValidityError(const validityError::Code& errorCode) const;

	ConsumptionService& consumptionService;
	PersonRepository* personRepo;
};