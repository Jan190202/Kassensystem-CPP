#include "PayTabExportDialog.h"
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QString>
#include <QStandardPaths>

PayTabExportDialog::PayTabExportDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Ausstände exportieren");
	
	// export option buttons
	auto* btnClipboard = new QPushButton("Zwischenablage");
	auto* btnCSV = new QPushButton("CSV");
	auto* btnWeb = new QPushButton("Webadresse");

	// disable buttons when their functionality is not yet implemented
	btnWeb->setEnabled(false);

	auto* exportLayout = new QVBoxLayout();
	exportLayout->addWidget(btnClipboard);
	exportLayout->addWidget(btnCSV);
	exportLayout->addWidget(btnWeb);

	auto* exportBox = new QGroupBox();
	exportBox->setTitle("Export-Typ");
	exportBox->setLayout(exportLayout);

	// sorting variable buttons
	auto* rBtnName = new QRadioButton("nach Namen");
	auto* rBtnDebt = new QRadioButton("nach Ausstand");
	auto* rBtnGroup = new QButtonGroup();

	rBtnName->setChecked(true);
	rBtnGroup->addButton(rBtnName);
	rBtnGroup->addButton(rBtnDebt);
	
	auto* sortLayout = new QVBoxLayout();
	sortLayout->addWidget(rBtnName);
	sortLayout->addWidget(rBtnDebt);

	auto* sortBox = new QGroupBox();
	sortBox->setTitle("Sortierung");
	sortBox->setLayout(sortLayout);
		
	auto* mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(sortBox);
	mainLayout->addWidget(exportBox);

	connect(btnClipboard, &QPushButton::clicked, this, [&]() 
		{
			inputs.option = ExportOption::clipboard;
			this->accept();
		});
	connect(btnCSV, &QPushButton::clicked, this, [&]() 
		{
			inputs.savePath = getPath();
			inputs.option = ExportOption::csv;
			this->accept();
		});
	connect(btnWeb, &QPushButton::clicked, this, [&]() 
		{
			inputs.option = ExportOption::web;
			this->accept();
		});

	connect(rBtnName, &QPushButton::clicked, this, [&]() {inputs.var = SortingVariable::name; });
	connect(rBtnDebt, &QPushButton::clicked, this, [&]() {inputs.var = SortingVariable::debt; });
}

PayTabExportDialog::Inputs PayTabExportDialog::getInputs() const
{
	return inputs;
}

std::string PayTabExportDialog::getPath()
{
	const QString caption = "Speichern als";
	const QDir desktopDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	const QString dir = desktopDir.filePath("Ausstände.csv");
	const QString filter = "*.csv";
	QString selectedFilter = "*.csv";
	const QFileDialog::Options options;

	QString pathQ = QFileDialog::getSaveFileName(this, caption, dir ,filter, &selectedFilter, options);
	
	// force csv format
	std::string path = pathQ.toStdString();
	if (!path.ends_with(".csv")) path += ".csv";

	qDebug() << QString::fromStdString(path);

	return path;
}