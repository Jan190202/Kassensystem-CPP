#pragma once
#include "domain/SessionController.h"
#include "data/sqlite/storage/SyncManager.h"

class SqliteSessionController : public SessionController
{
public:
	SqliteSessionController(SyncManager& syncManager);
	virtual ~SqliteSessionController() = default;

	virtual void save() override;
	virtual void sync() override;
	virtual void close() override;
private:
	SyncManager& syncManager;
};