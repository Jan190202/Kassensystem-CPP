#include "QtConversions.h"
#include <algorithm>

#include <QDebug>

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

	QString extractMonth(QDate date)
	{
		std::string m;

		switch (date.month())
		{
			case 1: m = "Januar"; break;
			case 2: m = "Februar"; break;
			case 3: m = "März"; break;
			case 4: m = "April"; break;
			case 5: m = "Mai"; break;
			case 6: m = "Juni"; break;
			case 7: m = "Juli"; break;
			case 8: m = "August"; break;
			case 9: m = "September"; break;
			case 10: m = "Oktober"; break;
			case 11: m = "November"; break;
			case 12: m = "Dezember"; break;
		}

		return QString::fromStdString(m);
	}
}
