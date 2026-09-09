#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <qDebug>

namespace priceList
{
	struct Entries
	{
		double beer04 = 2.5;
		double beer05 = 3;
		double water = 2.5;
		double softdrink = 3;

		friend std::ostream& operator<<(std::ostream& out, const Entries& entry)
		{
			out << "beer05: " << entry.beer05 << ", "
				<< "beer05: " << entry.beer04 << ", "
				<< "softdrink: " << entry.softdrink << ", "
				<< "water: " << entry.water;
			return out;
		}

		friend QDebug operator<<(QDebug out, const Entries& entry)
		{
			std::ostringstream oss;
			oss << entry;
			out.nospace() << QString::fromStdString(oss.str());
			return out;
		}
	};

	Entries read();
}