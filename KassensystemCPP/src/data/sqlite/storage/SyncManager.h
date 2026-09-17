#pragma once

class SyncManager
{
public:
	SyncManager() = default;
	virtual ~SyncManager() = default;

	virtual void setup() = 0;
	virtual void sync() = 0;
};