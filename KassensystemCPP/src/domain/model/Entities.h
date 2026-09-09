#pragma once

#include "DomainTypes.h"

#include <string>
#include <QDate>
#include <ostream>
#include <cstdint>
#include <sstream>
#include <QDebug>
#include <QString>

namespace entry
{
	struct Balance
	{
		int64_t balanceEntryID;
		BalanceType type;
		std::string description;
		double amount;
		QDate dateBooked;
		QDate dateAdded;
		std::string comment;
		int64_t personID;

		friend std::ostream& operator<<(std::ostream& out, const Balance& entry)
		{
			out << "balanceEntryID: " << entry.balanceEntryID << ", "
				<< "type: " << static_cast<int>(entry.type) << ", "
				<< "description: " << entry.description << ", "
				<< "amount: " << entry.amount << ", "
				<< "dateBooked: " << entry.dateBooked.toString(Qt::ISODate).toStdString() << ", "
				<< "dateAdded: " << entry.dateAdded.toString(Qt::ISODate).toStdString() << ", "
				<< "comment: " << entry.comment << ", "
				<< "personID: " << entry.personID;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Balance& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct Consumption
	{
		int64_t consumptionEntryID;
		int64_t debtEntryID;
		int nBeer05, nBeer04, nSoftdrinks, nWater;
		double otherExpense;

		friend std::ostream& operator<<(std::ostream& out, const Consumption& entry)
		{
			out << "consumptionEntryID: " << entry.consumptionEntryID << ", "
				<< "debtEntryID: " << entry.debtEntryID << ", "
				<< "nBeer05: " << entry.nBeer05 << ", "
				<< "nBeer04: " << entry.nBeer04 << ", "
				<< "nSoftdrinks: " << entry.nSoftdrinks << ", "
				<< "nWater: " << entry.nWater << ", "
				<< "otherExpense: " << entry.otherExpense;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Consumption& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct Debt
	{
		int64_t debtEntryID;
		int64_t personID;
		QDate date;
		double amount;
		double foreignShare = 0.85;

		friend std::ostream& operator<<(std::ostream& out, const Debt& entry)
		{
			out << "debtEntryID: " << entry.debtEntryID << ", "
				<< "personID: " << entry.personID << ", "
				<< "date: " << entry.date.toString(Qt::ISODate).toStdString() << ", "
				<< "amount: " << entry.amount << ", "
				<< "foreignShare: " << entry.foreignShare;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Debt& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct Outstanding
	{
		int64_t debtEntryID;
		QDate date;
		double amount;
		double remaining;

		friend std::ostream& operator<<(std::ostream& out, const Outstanding& entry)
		{
			out << "debtEntryID: " << entry.debtEntryID << ", "
				<< "date: " << entry.date.toString(Qt::ISODate).toStdString() << ", "
				<< "amount: " << entry.amount << ", "
				<< "remaining: " << entry.remaining;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Outstanding& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct Payment
	{
		int64_t paymentEntryID;
		int64_t personID;
		QDate date;
		double amount;
		OverpaymentDisposition overpaymentType;

		friend std::ostream& operator<<(std::ostream& out, const Payment& entry)
		{
			out << "paymentEntryID: " << entry.paymentEntryID << ", "
				<< "personID: " << entry.personID << ", "
				<< "date: " << entry.date.toString(Qt::ISODate).toStdString() << ", "
				<< "amount: " << entry.amount << ", "
				<< "overpaymentType: " << static_cast<int>(entry.overpaymentType);

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Payment& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct PaymentAllocation
	{
		int64_t paymentAllocationEntryID;
		int64_t debtEntryID;
		int64_t paymentEntryID;
		double amount;

		friend std::ostream& operator<<(std::ostream& out, const PaymentAllocation& entry)
		{
			out << "paymentAllocationEntryID: " << entry.paymentAllocationEntryID << ", "
				<< "debtEntryID: " << entry.debtEntryID << ", "
				<< "paymentEntryID: " << entry.paymentEntryID << ", "
				<< "amount: " << entry.amount;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const PaymentAllocation& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct Credit
	{
		int64_t creditEntryID;
		int64_t personID;
		QDate date;
		double amount;
		std::string description;

		friend std::ostream& operator<<(std::ostream& out, const Credit& entry)
		{
			out << "creditEntryID: " << entry.creditEntryID << ", "
				<< "personID: " << entry.personID << ", "
				<< "date: " << entry.date.toString(Qt::ISODate).toStdString() << ", "
				<< "amount: " << entry.amount << ", "
				<< "description: " << entry.description;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Credit& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct Settlement
	{
		int64_t settlementID;
		QDate date;
		double amount;

		friend std::ostream& operator<<(std::ostream& out, const Settlement& entry)
		{
			out << "settlementID: " << entry.settlementID << ", "
				<< "date: " << entry.date.toString(Qt::ISODate).toStdString() << ", "
				<< "amount: " << entry.amount;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const Settlement& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct SettlementAllocation
	{
		int64_t settlementAllocationID;
		int64_t debtEntryID;
		int64_t settlementID;
		double amount;

		friend std::ostream& operator<<(std::ostream& out, const SettlementAllocation& entry)
		{
			out << "settlementAllocationID: " << entry.settlementAllocationID << ", "
				<< "debtEntryID: " << entry.debtEntryID << ", "
				<< "settlementID: " << entry.settlementID << ", "
				<< "amount: " << entry.amount;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const SettlementAllocation& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};
}

namespace registerFinancials
{
	struct State
	{
		QDate date;
		double cash, savings, ownCash, foreignCash;
	};

	struct Report
	{
		registerFinancials::State stateBefore, stateAfter;
		double savingsDiff, cashDiff;
		double totalEarnings, totalSpendings;
	};
}

class Person
{
public:
	Person(const std::string& firstName, const std::string& lastName, int64_t id, const std::string& nickName = "", const std::string& info = "");
	std::string getFirstName() const;
	std::string getLastName() const;
	std::string getFullName() const;
	std::string getNickName() const;
	std::string getInfo() const;
	std::string getFullSpecifier() const;
	int64_t getID() const;

	friend std::ostream& operator<<(std::ostream& out, const Person& person);
	friend QDebug operator<<(QDebug out, const Person& person);
private:
	std::string firstName, lastName, nickName, info;
	int64_t id;
};