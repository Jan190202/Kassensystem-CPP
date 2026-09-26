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
	// rule: previous -> unknown -> any date -> subsequent
	
	using enum RegisterDate::Special;
	bool isSpecialL = this->isSpecial();
	bool isSpecialR = regDate.isSpecial();

	if (!isSpecialL && !isSpecialR)
	{
		return this->date() < regDate.date();
	}
	else if (isSpecialL && !isSpecialR)
	{
		switch (this->special())
		{
		case previous: return true;
		case unknown: return true;
		case subsequent: return false;
		}
	}
	else if (!isSpecialL && isSpecialR)
	{
		switch (regDate.special())
		{
		case previous: return false;
		case unknown: return false;
		case subsequent: return true;
		}
	}

	auto specialL = this->special();
	auto specialR = regDate.special();
	
	switch (specialL)
	{
	case previous:
		switch (specialR)
		{
		case previous:   return false;
		case unknown:    return true;
		case subsequent: return true;
		}
	case unknown:
		switch (specialR)
		{
		case previous:   return false;
		case unknown:    return false;
		case subsequent: return true;
		}
	case subsequent:
		switch (specialR)
		{
		case previous:   return false;
		case unknown:    return false;
		case subsequent: return false;
		}
	}

	std::unreachable();
}

bool RegisterDate::operator<=(const RegisterDate& regDate) const
{
	using enum RegisterDate::Special;
	bool isSpecialL = this->isSpecial();
	bool isSpecialR = regDate.isSpecial();

	if (!isSpecialL && !isSpecialR)
		if (this->date() == regDate.date())
			return true;

	if (isSpecialL && isSpecialR)
		if (this->special() == regDate.special())
			return true;

	return operator<(regDate);
}

bool RegisterDate::operator>(const RegisterDate& regDate) const
{
	return !operator<=(regDate);
}

bool RegisterDate::operator>=(const RegisterDate& regDate) const
{
	return !operator<(regDate);
}