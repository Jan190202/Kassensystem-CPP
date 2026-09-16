#pragma once
#include "data/sqlite/SqliteDatabase.h"
#include <QString>
#include <QStandardPaths>
#include <string>
#include <filesystem>

class SyncManager
{
public:
	SyncManager();
	void setupDatabase();

	std::string getLocalDatabasePath() const;
	std::string getRemoteDatabasePath() const;
private:
	std::filesystem::path localDatabasePath;
	std::filesystem::path remoteDatabasePath;

	std::filesystem::path getOneDrivePath() const;
};