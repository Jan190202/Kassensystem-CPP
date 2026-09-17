#pragma once
#include "data/sqlite/storage/SyncManager.h"
#include <string>
#include <filesystem>

class OneDriveSyncManager : public SyncManager
{
public:
	OneDriveSyncManager() = default;
	virtual ~OneDriveSyncManager() = default; 

	virtual void setup() override;// create and cleanup working directories, pull remote database to local
	virtual void sync() override; // push the local database to remote and backup if changes were made
	virtual void cleanup() override; // delete local database to enforce a fresh one needs to be pulled next session

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