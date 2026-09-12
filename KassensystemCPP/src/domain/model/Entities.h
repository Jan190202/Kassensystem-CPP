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
	struct Person
	{
		std::string firstName, lastName, nickName, info;
		int64_t personEntryID;

		std::string getFirstName() const;
		std::string getLastName() const;
		std::string getFullName() const;
		std::string getNickName() const;
		std::string getInfo() const;
		std::string getFullSpecifier() const;

		friend std::ostream& operator<<(std::ostream& out, const Person& person)
		{
			out << "fullSpecifier: " << person.getFullSpecifier() << ", "
				<< "personEntryID: " << person.personEntryID;
			return out;
		}
		friend QDebug operator<<(QDebug out, const Person& person)
		{
			std::ostringstream ss;
			ss << person;
			out.nospace() << QString::fromStdString(ss.str());
			return out;
		}
	};

	struct Balance
	{
		int64_t balanceEntryID;
		BalanceType type;
		std::string description;
		double amount;
		QDate dateBooked;
		QDate dateAdded;
		std::string comment;
		int64_t personEntryID;

		friend std::ostream& operator<<(std::ostream& out, const Balance& entry)
		{
			out << "balanceEntryID: " << entry.balanceEntryID << ", "
				<< "type: " << static_cast<int>(entry.type) << ", "
				<< "description: " << entry.description << ", "
				<< "amount: " << entry.amount << ", "
				<< "dateBooked: " << entry.dateBooked.toString("dd.MM.yyyy").toStdString() << ", "
				<< "dateAdded: " << entry.dateAdded.toString("dd.MM.yyyy").toStdString() << ", "
				<< "comment: " << entry.comment << ", "
				<< "personEntryID: " << entry.personEntryID;

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
		int64_t personEntryID;
		QDate date;
		double amount;
		double foreignShare = 0.85;

		friend std::ostream& operator<<(std::ostream& out, const Debt& entry)
		{
			out << "debtEntryID: " << entry.debtEntryID << ", "
				<< "personEntryID: " << entry.personEntryID << ", "
				<< "date: " << entry.date.toString("dd.MM.yyyy").toStdString() << ", "
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
				<< "date: " << entry.date.toString("dd.MM.yyyy").toStdString() << ", "
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
		int64_t personEntryID;
		QDate date;
		double amount;
		OverpaymentDisposition overpaymentType;

		friend std::ostream& operator<<(std::ostream& out, const Payment& entry)
		{
			out << "paymentEntryID: " << entry.paymentEntryID << ", "
				<< "personEntryID: " << entry.personEntryID << ", "
				<< "date: " << entry.date.toString("dd.MM.yyyy").toStdString() << ", "
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
		int64_t personEntryID;
		QDate date;
		double amount;
		std::string description;

		friend std::ostream& operator<<(std::ostream& out, const Credit& entry)
		{
			out << "creditEntryID: " << entry.creditEntryID << ", "
				<< "personEntryID: " << entry.personEntryID << ", "
				<< "date: " << entry.date.toString("dd.MM.yyyy").toStdString() << ", "
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

	struct ShareSettlement
	{
		int64_t shareSettlementEntryID;
		QDate date;
		double amount;

		friend std::ostream& operator<<(std::ostream& out, const ShareSettlement& entry)
		{
			out << "settlementEntryID: " << entry.shareSettlementEntryID << ", "
				<< "date: " << entry.date.toString("dd.MM.yyyy").toStdString() << ", "
				<< "amount: " << entry.amount;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const ShareSettlement& entry)
		{
			std::ostringstream oss;
			oss << entry;
			debug.nospace() << QString::fromStdString(oss.str());
			return debug;
		}
	};

	struct ShareSettlementAllocation
	{
		int64_t shareSettlementAllocationEntryID;
		int64_t debtEntryID;
		int64_t shareSettlementEntryID;
		double amount;

		friend std::ostream& operator<<(std::ostream& out, const ShareSettlementAllocation& entry)
		{
			out << "settlementAllocationEntryID: " << entry.shareSettlementAllocationEntryID << ", "
				<< "debtEntryID: " << entry.debtEntryID << ", "
				<< "settlementEntryID: " << entry.shareSettlementEntryID << ", "
				<< "amount: " << entry.amount;

			return out;
		}

		friend QDebug operator<<(QDebug debug, const ShareSettlementAllocation& entry)
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

		friend std::ostream& operator<<(std::ostream& out, const State& state)
		{
			out << "date: " << state.date.toString("dd.MM.yyyy").toStdString() << ", "
				<< "savings: " << state.savings << ", "
				<< "cash: " << state.cash << ", "
				<< "foreignCash: " << state.foreignCash << ", "
				<< "ownCash: " << state.ownCash;
			return out;
		}

		friend QDebug operator<<(QDebug out, const State& state)
		{
			std::ostringstream oss;
			oss << state;
			out << QString::fromStdString(oss.str());
			return out;
		}
	};

	struct Report
	{
		registerFinancials::State stateBefore, stateAfter;
		double savingsDiff, cashDiff;
		double totalEarnings, totalSpendings;
	};
}

struct PriceList
{
	double beer04 = 2.5;
	double beer05 = 3;
	double water = 2.5;
	double softdrink = 3;

	friend std::ostream& operator<<(std::ostream& out, const PriceList& entry)
	{
		out << "beer05: " << entry.beer05 << ", "
			<< "beer05: " << entry.beer04 << ", "
			<< "softdrink: " << entry.softdrink << ", "
			<< "water: " << entry.water;
		return out;
	}

	friend QDebug operator<<(QDebug out, const PriceList& entry)
	{
		std::ostringstream oss;
		oss << entry;
		out.nospace() << QString::fromStdString(oss.str());
		return out;
	}
};