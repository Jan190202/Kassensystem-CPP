#include "SyncManager.h"
#include <QDatetime>
#include <QDebug>

namespace fs = std::filesystem;

SyncManager::SyncManager() {}

void SyncManager::setupDatabase()
{
	fs::path targetLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
	fs::path targetRemote = getOneDrivePath() / "KassensystemSVU";

	if (!fs::is_directory(targetLocal))
	{
		fs::create_directories(targetLocal);
	}
	if (!fs::is_directory(targetRemote))
	{
		fs::create_directories(targetRemote);
	}

	std::string databaseFileName = "registerData.db";
	localDatabasePath = (targetLocal / databaseFileName).make_preferred();
	remoteDatabasePath = (targetRemote / databaseFileName).make_preferred();

	cleanupLocalDir();
	pullFromRemote();
}

void SyncManager::sync()
{
	bool databaseChanged = true; // TBD: check if local database changed
	if (databaseChanged) 
	{
		pushToRemote();
		pushToBackup();
	}
}

void SyncManager::cleanupLocalDir()
{
	// clear for now
	for (const auto& entry : fs::directory_iterator(localDatabasePath.parent_path())) 
	{
		fs::remove_all(entry.path());
	};
}

void SyncManager::pullFromRemote()
{
	if (fs::exists(remoteDatabasePath))
	{
		fs::copy_file(remoteDatabasePath, localDatabasePath);
	}
	else
	{
		qDebug() << "Remote database not found!";
	}
}

void SyncManager::pushToRemote()
{
	fs::copy_file(localDatabasePath, remoteDatabasePath, fs::copy_options::overwrite_existing);
}

void SyncManager::pushToBackup()
{
	fs::path targetRemoteBackup = getOneDrivePath() / "KassensystemSVU" / "Backups";
	if (!fs::is_directory(targetRemoteBackup))
	{
		fs::create_directories(targetRemoteBackup);
	}

	std::string backupDatabaseFileName = "registerData_" + QDateTime::currentDateTime().toString("dd.MM.yy_hh.mm.ss").toStdString() + ".db"; // e.g. registerData_13.09.26_13.27.03.db
	remoteBackupDatabasePath = (targetRemoteBackup / backupDatabaseFileName).make_preferred();
	fs::copy_file(localDatabasePath, remoteBackupDatabasePath, fs::copy_options::overwrite_existing);
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

	qDebug() << "OneDrive path not found!";
	return fs::path(); // else, empty
}
