#pragma once
#include "domain/model/Entities.h"
#include <vector>

namespace exporter
{
	void toClipboard(const std::vector<exportType::PersonDebt>& entries);
	void toCSV(const std::vector<exportType::PersonDebt>& entries, const std::string& savePath);
	void toWeb(const std::vector<exportType::PersonDebt>& entries);
}