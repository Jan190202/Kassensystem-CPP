#pragma once
#include "domain/SessionController.h"
#include "data/sqlite/storage/SyncManager.h"

class SqliteSessionController : public SessionController
{
public:
	SqliteSessionController(SyncManager& syncManager);
	virtual ~SqliteSessionController() = default;

	virtual void save() const override;
	virtual void sync() const override;
	virtual void close() const override;
private:
	SyncManager& syncManager;
};