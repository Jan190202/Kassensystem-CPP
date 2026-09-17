#pragma once

class SessionController
{
public:
	SessionController() = default;
	virtual ~SessionController() = default;

	virtual void save() = 0;
	virtual void sync() = 0;
	virtual void close() = 0;
};