#pragma once

class SessionController
{
public:
	SessionController() = default;
	virtual ~SessionController() = default;

	virtual void save() const = 0;
	virtual void sync() const = 0;
	virtual void close() const = 0;
};