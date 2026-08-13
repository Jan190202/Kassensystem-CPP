#pragma once

#include <string>
#include <vector>
#include <QList>
#include <QString>

namespace Utils
{
	QList<QString> strVecToQStrList(const std::vector<std::string>& vec);
}