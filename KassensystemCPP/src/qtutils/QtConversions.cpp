#include "QtConversions.h"
#include <algorithm>

namespace QtUtils
{
	QList<QString> strVecToQStrList(const std::vector<std::string>& vec)
	{
		QList<QString> listQ;
		listQ.reserve(vec.size());
		for (const std::string& str : vec)
		{
			listQ.append(QString::fromStdString(str));
		}
		return listQ;
	}

	QString toCurrencyFormat(double value)
	{
		return QObject::tr("%1 \u20AC").arg(QString::number(value, 'f', 2));
	}
}
