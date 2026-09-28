#include "domain/model/Entities.h"

RegisterDate::RegisterDate(QDate date)
{
	this->data = date;
}

RegisterDate::RegisterDate(Special date)
{
	this->data = date;
}

bool RegisterDate::isSpecial() const
{
	return std::holds_alternative<Special>(this->data);
}

RegisterDate::Special RegisterDate::special() const
{
	return std::get<Special>(this->data);
}

QDate RegisterDate::date() const
{
	return std::get<QDate>(this->data);
}

std::string RegisterDate::toString() const
{
	if (!isSpecial())
		return this->date().toString("dd.MM.yyyy").toStdString();
	else
	{
		switch (this->special())
		{
		case RegisterDate::Special::unknown: return "unbekannt";
		case RegisterDate::Special::previous: return "vergangen";
		case RegisterDate::Special::subsequent: return "zukünftig";
		default: return "UNDEFINED";
		}
	}
}

QString RegisterDate::toQString() const
{
	return QString::fromStdString(toString());
}

QVariant RegisterDate::toSqlDateValue() const 
{
	return isSpecial() ? QVariant() : QVariant(date().toString(Qt::ISODate));
}

QVariant RegisterDate::toSqlSpecialValue() const 
{
	return isSpecial() ? QVariant(static_cast<int>(special())) : QVariant();
}

int RegisterDate::sortRank(const RegisterDate& regDate)
{
	// rule: previous (1) -> unknown (2) -> any date (3) -> subsequent (4)
	
	if (!regDate.isSpecial())
		return 3;
	else
	{
		switch (regDate.special())
		{
		case RegisterDate::Special::previous: return 1;
		case RegisterDate::Special::unknown: return 2;
		case RegisterDate::Special::subsequent: return 4;
		}
	}

	std::unreachable();
}

int RegisterDate::sortRank() const
{
	return sortRank(*this);
}

std::ostream& operator<<(std::ostream& out, const RegisterDate& regDate)
{
	if (std::holds_alternative<QDate>(regDate.data))
	{
		out << std::get<QDate>(regDate.data).toString("dd.MM.yyyy").toStdString();
	}
	else if (std::holds_alternative<RegisterDate::Special>(regDate.data))
	{
		switch (std::get<RegisterDate::Special>(regDate.data))
		{
		case RegisterDate::Special::unknown:
			out << "unbekannt";
			break;
		case RegisterDate::Special::previous:
			out << "vergangen";
			break;
		case RegisterDate::Special::subsequent:
			out << "zukünftig";
			break;
		}
	}

	return out;
}

QDebug operator<<(QDebug out, const RegisterDate& regDate)
{
	std::ostringstream ss;
	ss << regDate;
	out.nospace() << QString::fromStdString(ss.str());
	return out;
}

bool RegisterDate::operator<(const RegisterDate& regDate) const
{	
	if (!this->isSpecial() && !regDate.isSpecial())
		return this->date() < regDate.date();
	else
		return this->sortRank() < regDate.sortRank();
}

bool RegisterDate::operator<=(const RegisterDate& regDate) const
{
	if (!this->isSpecial() && !regDate.isSpecial())
		return this->date() <= regDate.date();
	else
		return this->sortRank() <= regDate.sortRank();
}

bool RegisterDate::operator>(const RegisterDate& regDate) const
{
	return !operator<=(regDate);
}

bool RegisterDate::operator>=(const RegisterDate& regDate) const
{
	return !operator<(regDate);
}