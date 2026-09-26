#include "domain/model/Entities.h"

RegisterDate::RegisterDate(QDate date)
{
	this->date = date;
}

RegisterDate::RegisterDate(SpecialDate date)
{
	this->date = date;
}

bool RegisterDate::isSpecial() const
{
	return std::holds_alternative<SpecialDate>(this->date);
}

std::ostream& operator<<(std::ostream& out, const RegisterDate& rDate)
{
	if (std::holds_alternative<QDate>(rDate.date))
	{
		out << std::get<QDate>(rDate.date);
	}
	else if (std::holds_alternative<RegisterDate::SpecialDate>(rDate.date))
	{
		switch (std::get<RegisterDate::SpecialDate>(rDate.date))
		{
		case RegisterDate::SpecialDate::Unknown:
			out << "unbekannt";
			break;
		case RegisterDate::SpecialDate::Previous:
			out << "vergangen";
			break;
		case RegisterDate::SpecialDate::Subsequent:
			out << "zukünftig";
			break;
		}
	}

	return out;
}

QDebug operator<<(QDebug out, const RegisterDate& rDate)
{
	std::ostringstream ss;
	ss << rDate;
	out.nospace() << QString::fromStdString(ss.str());
	return out;
}