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

	QString extractMonth(QDate date)
	{
		std::string m;
		
		switch (date.month())
		{
			case 1: m = "Januar";
			case 2: m = "Februar";
			case 3: m = "März";
			case 4: m = "April";
			case 5: m = "Mai";
			case 6: m = "Juni";
			case 7: m = "Juli";
			case 8: m = "August";
			case 9: m = "September";
			case 10: m = "Oktober";
			case 11: m = "November";
			case 12: m = "Dezember";
		}

		return QString::fromStdString(m);
	}
}
