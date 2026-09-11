#pragma once
#include <vector>
#include <string>

class RepositoryController
{
public:
	RepositoryController() = default;
	virtual ~RepositoryController() = default;

	virtual std::vector<std::string> getUnsavedChanges() const = 0;
	virtual void save() = 0;
	virtual void sync() = 0;
};