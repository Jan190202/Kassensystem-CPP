#pragma once
#include <QIcon>
#include <string>

namespace iconLoader
{
	QIcon getIcon(const std::string& fileNameStr, bool isThemeDependent = true, const std::string& relPath = "assets");
}
