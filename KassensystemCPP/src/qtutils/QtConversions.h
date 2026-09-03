#pragma once

#include <string>
#include <vector>
#include <QList>
#include <QString>
#include <QObject>
#include <QDate>
#include <domain/model/Entities.h>

namespace QtUtils
{
	QList<QString> strVecToQStrList(const std::vector<std::string>& vec);

	// template for generating a QList<QString> for any getter of person; templates need to be defined in header in c++
	QList<QString> personVecToQStrList(const std::vector<Person>& personVec, auto projection) // auto projection carries pointer to member function in Person, like &Person::getFullName
	{
		QList<QString> listQ;
		listQ.reserve(personVec.size()); // x.reserve(int) instead of QList<> x(int) as expensive string constructor would be called for every index, else

		std::ranges::transform(
			personVec,
			std::back_inserter(listQ), // inserter instead of listQ.begin() as index has size 0 because of reserve() --> push_back() needs to be invoked
			[](const std::string& str) { return QString::fromStdString(str); },
			projection
		);

		return listQ;
	}

	QString toCurrencyFormat(double value);

	QString extractMonth(QDate);
}