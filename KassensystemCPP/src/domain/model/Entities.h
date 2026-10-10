#pragma once
#include "DomainTypes.h"
#include "common/Utils.h"
#include <QDebug>
#include <QString>
#include <QDate>
#include <QDateTime>
#include <string>
#include <ostream>
#include <cstdint>
#include <variant>
#include <map>

class RegisterDate
{
public:
	enum class Special
	{
		unknown, previous, subsequent
	};

	// needed for SQL clause construction as long as static reflections aren't possible
	static constexpr std::array<Special, 3> allSpecials{ Special::previous, Special::unknown, Special::subsequent };

	RegisterDate() = default;
	RegisterDate(QDate date);
	RegisterDate(Special date);

	bool isSpecial() const;
	Special special() const;
	QDate date() const;

	std::string toString() const;
	QString toQString() const;
	
	QVariant toSqlDateValue() const;
	QVariant toSqlSpecialValue() const;

	int sortRank() const;
	static int sortRank(const RegisterDate& regDate);

	bool operator<(const RegisterDate& regDate) const;
	bool operator<=(const RegisterDate& regDate) const;
	bool operator>(const RegisterDate& regDate) const;
	bool operator>=(const RegisterDate& regDate) const;

	friend std::ostream& operator<<(std::ostream&, const RegisterDate& regDate);

	friend QDebug operator<<(QDebug out, const RegisterDate& regDate);

private:
	std::variant<QDate, Special> data;
};

namespace entry
{
	struct Person
	{
		int64_t personEntryID;
		std::string firstName, lastName, nickName, info;

		std::string getFirstName() const;
		std::string getLastName() const;
		std::string getFullName() const;
		std::string getNickName() const;
		std::string getInfo() const;
		std::string getFullSpecifier() const;

		std::string toString() const
		{
			return std::format("{} ({})", getFullName(), personEntryID);
		}

		friend std::ostream& operator<<(std::ostream& out, const Person& person)
		{
			out << "fullSpecifier: " << person.getFullSpecifier() << ", " << "personEntryID: " << std::to_string(person.personEntryID);
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
		RegisterDate dateBooked;
		QDate dateAdded;
		std::string comment;
		std::optional<Person> person;

		friend std::ostream& operator<<(std::ostream& out, const Balance& entry)
		{
			out << "balanceEntryID: " << entry.balanceEntryID << ", "
				<< "type: " << static_cast<int>(entry.type) << ", "
				<< "description: " << entry.description << ", "
				<< "amount: " << entry.amount << ", "
				<< "dateBooked: " << entry.dateBooked << ", "
				<< "dateAdded: " << entry.dateAdded.toString("dd.MM.yyyy").toStdString() << ", "
				<< "comment: " << entry.comment << ", "
				<< "person: " << (entry.person.has_value() ? entry.person.value().toString() : "NULL");

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
		Person person;
		RegisterDate dateBooked;
		QDate dateAdded;
		double amount;
		double foreignShare = 0.85;

		friend std::ostream& operator<<(std::ostream& out, const Debt& entry)
		{
			out << "debtEntryID: " << entry.debtEntryID << ", "
				<< "person: " << entry.person.toString() << ", "
				<< "dateBooked: " << entry.dateBooked << ", "
				<< "dateAdded: " << entry.dateAdded.toString("dd.MM.yyyy").toStdString() << ", "
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
		RegisterDate dateBooked;
		double amount;
		double remaining;

		friend std::ostream& operator<<(std::ostream& out, const Outstanding& entry)
		{
			out << "debtEntryID: " << entry.debtEntryID << ", "
				<< "dateBooked: " << entry.dateBooked << ", "
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
		Person person;
		QDate dateAdded;
		double amount;
		std::string comment;
		OverpaymentDisposition overpaymentType;

		friend std::ostream& operator<<(std::ostream& out, const Payment& entry)
		{
			out << "paymentEntryID: " << entry.paymentEntryID << ", "
				<< "person: " << entry.person.toString() << ", "
				<< "dateAdded: " << entry.dateAdded.toString("dd.MM.yyyy").toStdString() << ", "
				<< "amount: " << entry.amount << ", "
				<< "comment: " << entry.comment << ", "
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
		Person person;
		RegisterDate dateBooked;
		QDate dateAdded;
		double amount;
		std::string description;

		friend std::ostream& operator<<(std::ostream& out, const Credit& entry)
		{
			out << "creditEntryID: " << entry.creditEntryID << ", "
				<< "person: " << entry.person.toString() << ", "
				<< "dateBooked: " << entry.dateBooked << ", "
				<< "dateAdded: " << entry.dateAdded.toString("dd.MM.yyyy").toStdString() << ", "
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
		QDate dateAdded;
		double amount;
		std::string comment;

		friend std::ostream& operator<<(std::ostream& out, const ShareSettlement& entry)
		{
			out << "settlementEntryID: " << entry.shareSettlementEntryID << ", "
				<< "dateAdded: " << entry.dateAdded.toString("dd.MM.yyyy").toStdString() << ", "
				<< "amount: " << entry.amount << ", "
				<< "comment: " << entry.comment;

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

namespace exportType
{
	struct PersonDebt
	{
		std::string name;
		double debt;
	};
}

namespace registerFinancials
{
	struct State
	{
		QDate date;
		double cash = 0.0;			// counted cash (review) or expected cash (current state)
		double foreignCash = 0.0;	
		double ownCash = 0.0;		
		double debt = 0.0;			
		double credit = 0.0;		
		double savings = 0.0;		

		friend std::ostream& operator<<(std::ostream& out, const State& state)
		{
			out << "date: " << state.date.toString("dd.MM.yyyy").toStdString() << ", "
				<< "savings: " << state.savings << ", "
				<< "cash: " << state.cash << ", "
				<< "foreignCash: " << state.foreignCash << ", "
				<< "ownCash: " << state.ownCash << ", "
				<< "debt: " << state.debt << ", "
				<< "credit: " << state.credit;
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

	// begin + added - removed = end; endDirect is read from the database independently,
	// so the roll-forward can be checked against it
	struct RollForwardRow
	{
		double begin = 0.0;
		double added = 0.0;
		double removed = 0.0;
		double endDirect = 0.0;
	};

	struct ConsumptionSummary
	{
		double totalConsumption = 0.0;	
		double departmentShare = 0.0;	

		RollForwardRow debt;			// added: consumption, removed: paid
		RollForwardRow foreignShare;	// added: club share of consumption, removed: settled with the club
		RollForwardRow credit;			// added: deposited credit, removed: credit used up
	};

	// Cash-flow view of the period (journal entries and cash events only)
	struct Details
	{
		double departmentEarnings = 0.0, departmentSpendings = 0.0;
		double paidDebt = 0.0, settledValue = 0.0, depositedCredit = 0.0;
		double cashCorrection = 0.0;	// counted - expected cash at the end review; 0 for the current period
	};

	struct PeriodReport
	{
		State start, end;

		QDate periodStart;						// entries are filtered by dateBooked > periodStart ...
		std::optional<QDate> periodEnd;			// ... and <= periodEnd (nullopt = open end, current period)

		bool isCurrent = false;					// period runs from the last review to now
		bool changedSinceReview = false;		// recomputed expected cash != expectedCash stored at the end review

		double totalEarnings = 0.0, totalSpendings = 0.0;	// journal entries only

		Details details;
		ConsumptionSummary consumption;
	};
}

class PendingChangeLog
{
public:
	enum class ChangeType
	{
		add, remove, mutate
	};

	enum class TextFormat
	{
		plain, rich
	};

	struct Field
	{
		std::string label;
		std::string value;
		bool quoted = false;
	};

	struct Change
	{
		QDateTime time;
		std::optional<ChangeType> type;
		std::string subject;
		std::vector<Field> fields;
		std::string message;

		bool isFreeText() const { return subject.empty(); }
		std::string plainText() const;
	};

	PendingChangeLog() = default;

	void record(ChangeType, const entry::Balance&);
	void record(ChangeType, const entry::Debt&, const std::optional<entry::Consumption>&);
	void record(ChangeType, const entry::Credit&);
	void record(ChangeType, const entry::Payment&);
	void record(ChangeType, const entry::Person&);
	void record(ChangeType, const entry::ShareSettlement&);
	void record(const std::string& description, std::optional<ChangeType> type = std::nullopt);

	bool hasPendingChanges() const;
	const std::vector<Change>& pendingChanges() const;
	std::string printPendingChanges(TextFormat format = TextFormat::plain) const;
	void clear();
private:
	std::vector<Change> log;

	void addLogEntry(ChangeType type, std::string subject, std::vector<Field> fields);
};

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
