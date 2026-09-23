#pragma once
#include "domain/model/Entities.h"
#include <vector>

namespace exporter
{
	void toClipboard(const std::vector<exportType::personDebt>& entries);
	void toCSV(const std::vector<exportType::personDebt>& entries, const std::string& savePath);
	void toWeb(const std::vector<exportType::personDebt>& entries);
}