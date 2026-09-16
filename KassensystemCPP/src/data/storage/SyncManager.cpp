#include "SyncManager.h"
#include <filesystem>

namespace fs = std::filesystem;

SyncManager::SyncManager()
{
	localDatabasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
	remoteDatabasePath = "";

	QDir dir(QString::fromStdString(dataDir));

	if (!dir.exists())
	{
		dir.mkpath(".");
	}

	std::string dbPath = dataDir + "/mydatabase.db";

}

void SyncManager::setupDatabase()
{

	std::string dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();

	QDir dir(QString::fromStdString(dataDir));

	if (!dir.exists())
	{
		dir.mkpath(".");
	}

	std::string dbPath = dataDir + "/mydatabase.db";

}