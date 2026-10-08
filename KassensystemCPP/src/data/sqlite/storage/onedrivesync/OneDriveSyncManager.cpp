#include "OneDriveSyncManager.h"
#include "system/SystemConfig.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>

namespace fs = std::filesystem;

void OneDriveSyncManager::setup()
{
	remoteFolderName = "KassensystemSVU";
	if (systemConfig::isDebug())
		remoteFolderName += "_Debug";

	fs::path targetLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
	fs::path targetRemote = getOneDrivePath() / remoteFolderName;

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
	pushToRemote();
	pushToBackup();
}

void OneDriveSyncManager::cleanup()
{
	cleanupLocalDir();
}

void OneDriveSyncManager::cleanupLocalDir()
{
	// clear for now
	for (const auto& entry : fs::directory_iterator(localDatabasePath.parent_path()))
	{
		fs::remove_all(entry.path());
	}
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

bool OneDriveSyncManager::vacuumInto(const fs::path& target)
{
	bool ok = false;

	{
		QSqlDatabase syncDb = QSqlDatabase::addDatabase("QSQLITE", "syncConnection");
		syncDb.setDatabaseName(QString::fromStdString(localDatabasePath.string()));

		if (syncDb.open())
		{
			QSqlQuery query(syncDb);
			const QString targetPath = QString::fromStdString(target.string()).replace("'", "''");

			ok = query.exec("VACUUM INTO '" + targetPath + "'");
			if (!ok)
				qDebug() << "VACUUM INTO failed:" << query.lastError().text();
		}
		else
		{
			qDebug() << "Could not open sync connection:" << syncDb.lastError().text();
		}

		syncDb.close();
	}

	QSqlDatabase::removeDatabase("syncConnection");
	return ok;
}

void OneDriveSyncManager::pushToRemote()
{
	fs::path tmpPath = remoteDatabasePath;
	tmpPath += ".tmp";

	std::error_code ec;
	fs::remove(tmpPath, ec);

	if (!vacuumInto(tmpPath))
	{
		fs::remove(tmpPath, ec);
		return;
	}

	fs::rename(tmpPath, remoteDatabasePath, ec);
	if (ec)
	{
		qDebug() << "Replacing remote database failed:" << QString::fromStdString(ec.message());
		fs::remove(tmpPath, ec);
	}
}

void OneDriveSyncManager::pushToBackup()
{
	fs::path targetRemoteBackup = getOneDrivePath() / remoteFolderName / "Backups";
	if (!fs::is_directory(targetRemoteBackup))
	{
		fs::create_directories(targetRemoteBackup);
	}

	std::string backupDatabaseFileName = "registerData_" + QDateTime::currentDateTime().toString("dd.MM.yy_hh'h'mm'min'ss's'").toStdString() + ".db"; // e.g. registerData_13.09.26_13h27min03s.db
	remoteBackupDatabasePath = (targetRemoteBackup / backupDatabaseFileName).make_preferred();

	if (fs::exists(remoteBackupDatabasePath)) // in the rare case multiple backups are made in the same second, the last one wins
	{
		fs::remove(remoteBackupDatabasePath);
	}

	vacuumInto(remoteBackupDatabasePath);
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