#include "SqliteSessionController.h"
#include <QSqlDatabase>

SqliteSessionController::SqliteSessionController(SyncManager& syncManager) : syncManager(syncManager) {}

void SqliteSessionController::save() const
{
	// save transactions that were made to local database and open new transaction directly
	QSqlDatabase::database().commit();
	QSqlDatabase::database().transaction();
}

void SqliteSessionController::sync() const
{
	syncManager.sync();
}

void SqliteSessionController::close() const
{
	QSqlDatabase::database().close();
	syncManager.cleanup();
}
