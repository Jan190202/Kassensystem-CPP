#include "Utils.h"

namespace Utils
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
}
