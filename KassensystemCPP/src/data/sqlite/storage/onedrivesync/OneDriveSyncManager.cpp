#include "OneDriveSyncManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDatetime>
#include <QDebug>

namespace fs = std::filesystem;

void OneDriveSyncManager::setup()
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

void OneDriveSyncManager::sync()
{
	bool databaseChanged = true; // TBD: check if local database changed
	if (databaseChanged) 
	{
		pushToRemote();
		pushToBackup();
	}
}

void OneDriveSyncManager::cleanupLocalDir()
{
	// clear for now
	for (const auto& entry : fs::directory_iterator(localDatabasePath.parent_path())) 
	{
		fs::remove_all(entry.path());
	};
}

void OneDriveSyncManager::pullFromRemote()
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

void OneDriveSyncManager::pushToRemote()
{
	fs::remove(remoteDatabasePath);
	
	// safe upload with possibly open transaction, needs a seperate connection (not the one with open transaction)
	QSqlDatabase syncDb = QSqlDatabase::addDatabase("QSQLITE", "syncConnection");
	syncDb.setDatabaseName(QString::fromStdString(localDatabasePath.string()));
	syncDb.open();

	QSqlQuery(syncDb).exec("VACUUM INTO '" + QString::fromStdString(remoteDatabasePath.string()) + "'");

	syncDb.close();
	QSqlDatabase::removeDatabase("syncConnection");
}

void OneDriveSyncManager::pushToBackup()
{
	fs::path targetRemoteBackup = getOneDrivePath() / "KassensystemSVU" / "Backups";
	if (!fs::is_directory(targetRemoteBackup))
	{
		fs::create_directories(targetRemoteBackup);
	}

	std::string backupDatabaseFileName = "registerData_" + QDateTime::currentDateTime().toString("dd.MM.yy_hh'h'mm'min'ss's'").toStdString() + ".db"; // e.g. registerData_13.09.26_13.27.03.db
	remoteBackupDatabasePath = (targetRemoteBackup / backupDatabaseFileName).make_preferred();

	if (fs::exists(remoteBackupDatabasePath)) // in the rare case multiple backups are made in the same second, the last one wins
	{
		fs::remove(remoteBackupDatabasePath);
	}

	// safe upload with possibly open transaction, needs a seperate connection (not the one with open transaction)
	QSqlDatabase syncDb = QSqlDatabase::addDatabase("QSQLITE", "syncConnection");
	syncDb.setDatabaseName(QString::fromStdString(localDatabasePath.string()));
	syncDb.open();

	QSqlQuery(syncDb).exec("VACUUM INTO '" + QString::fromStdString(remoteBackupDatabasePath.string()) + "'");

	syncDb.close();
	QSqlDatabase::removeDatabase("syncConnection");
}

std::string OneDriveSyncManager::getLocalDatabasePath() const
{
	return localDatabasePath.string();
}

std::string OneDriveSyncManager::getRemoteDatabasePath() const
{
	return remoteDatabasePath.string();
}

fs::path OneDriveSyncManager::getOneDrivePath() const
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
