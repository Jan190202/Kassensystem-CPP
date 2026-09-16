#include "SyncManager.h"

namespace fs = std::filesystem;

SyncManager::SyncManager() {}

void SyncManager::setupDatabase()
{
	std::string databaseFileName = "registerData.db";
	
	fs::path targetLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();

	if (!fs::is_directory(targetLocal))
	{
		fs::create_directories(targetLocal);
	}

	localDatabasePath = (targetLocal / databaseFileName).make_preferred();
	remoteDatabasePath = (getOneDrivePath() / databaseFileName).make_preferred();
}

std::string SyncManager::getLocalDatabasePath() const
{
	return localDatabasePath.string();
}

std::string SyncManager::getRemoteDatabasePath() const
{
	return remoteDatabasePath.string();
}

fs::path SyncManager::getOneDrivePath() const
{
	// general or private onedrive
	if (const char* env_p = std::getenv("OneDrive")) 
	{
		return fs::path(env_p);
	}

	// business onedrive
	if (const char* env_p = std::getenv("OneDriveCommercial")) 
	{
		return fs::path(env_p);
	}

	// fall back: guess standard path
	if (const char* user_profile = std::getenv("USERPROFILE")) 
	{
		fs::path fallback = fs::path(user_profile) / "OneDrive";
		if (fs::exists(fallback)) 
		{
			return fallback;
		}
	}

	return fs::path(); // else, empty
}