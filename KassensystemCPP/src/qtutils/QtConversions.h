#pragma once

#include <string>
#include <vector>
#include <QList>
#include <QString>
#include <QObject>

namespace QtUtils
{
	QList<QString> strVecToQStrList(const std::vector<std::string>& vec);
	QString toCurrencyFormat(double value);
}