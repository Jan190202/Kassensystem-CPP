#pragma once
#include <QDialog>
#include <optional>
#include <string>

class PayTabExportDialog : public QDialog
{
	Q_OBJECT
public:
	enum class ExportOption
	{
		clipboard, csv, web
	};

	enum class SortingVariable
	{
		name, debt
	};

	struct Inputs
	{
		ExportOption option;
		SortingVariable var;
		std::optional<std::string> savePath;
	};

	PayTabExportDialog(QWidget* parent);

	Inputs getInputs() const;

private:
	Inputs inputs;

	std::string getPath();
};