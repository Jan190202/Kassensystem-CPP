#pragma once
#include "data/sqlite/SqliteDatabase.h"
#include <string>
#include <QString>
#include <QStandardPaths>

class SyncManager
{
public:
	SyncManager();
	void setupDatabase();

	std::string getLocalDatabasePath();
	std::string getRemoteDatabasePath();

	void setLocalDatabasePath(std::string path);
	void setRemoteDatabasePath(std::string path);
private:
	std::string localDatabasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
	std::string remoteDatabasePath;
};