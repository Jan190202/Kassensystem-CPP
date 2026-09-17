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
	void setupDatabase(); // create and cleanup working directories, pull remote database to local
	void sync(); // push the local database to remote and backup if changes were made

	std::string getLocalDatabasePath() const;
	std::string getRemoteDatabasePath() const;
private:
	std::filesystem::path localDatabasePath;
	std::filesystem::path remoteDatabasePath;
	std::filesystem::path remoteBackupDatabasePath;

	std::filesystem::path getOneDrivePath() const;
	void cleanupLocalDir();
	void pullFromRemote();
	void pushToRemote();
	void pushToBackup();
};