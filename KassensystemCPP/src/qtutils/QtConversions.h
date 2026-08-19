#pragma once

#include <string>
#include <vector>
#include <QList>
#include <QString>

namespace QtUtils
{
	QList<QString> strVecToQStrList(const std::vector<std::string>& vec);
}