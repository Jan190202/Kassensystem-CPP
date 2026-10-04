#include "domain/model/Entities.h"
#include <format>

bool PendingChangeLog::hasPendingChanges() const
{
	return !log.empty();
}

const std::vector<PendingChangeLog::Change>& PendingChangeLog::pendingChanges() const
{
	return log;
}

void PendingChangeLog::record(ChangeType type, const entry::Balance& entry)
{
	std::string description = std::format("Bilanz {} - Beschreibung: \"{}\", Betrag: {}, ausgelegt von: {}", actionVerb.at(type), entry.description, entry.amount, entry.personEntryID.has_value() ? std::to_string(entry.personEntryID.value()) : "-");
	
	addLogEntry(description);
}

void PendingChangeLog::record(ChangeType type, const entry::Debt& dEntry, const std::optional<entry::Consumption>& cEntry)
{
	std::string description = std::format("Schulden {} - Person: {}, Betrag: {}", actionVerb.at(type), dEntry.personEntryID, dEntry.amount);
	
	if (cEntry.has_value()) 
		description += std::format(", Mengen: {}x, {}x, {}x, {}x, {:.2f}€", cEntry.value().nBeer05, cEntry.value().nBeer04, cEntry.value().nSoftdrinks, cEntry.value().nWater, cEntry.value().otherExpense);
	
	addLogEntry(description);
}

void PendingChangeLog::record(ChangeType type, const entry::Credit& entry)
{
	std::string description = std::format("Guthaben {} - Person: {}, Betrag: {}", actionVerb.at(type), entry.personEntryID, entry.amount);

	addLogEntry(description);
}

void PendingChangeLog::record(ChangeType type, const entry::Payment& entry)
{
	std::string description = std::format("Zahlung {} - Person: {}, Betrag: {}, Überlauf: ", actionVerb.at(type), entry.personEntryID, entry.amount, entry.overpaymentType==OverpaymentDisposition::credit ? "Guthaben" : "Trinkgeld");

	addLogEntry(description);
}

void PendingChangeLog::record(ChangeType type, const entry::Person& entry)
{
	std::string description = std::format("Person {} - ", actionVerb.at(type), entry.getFullSpecifier());

	addLogEntry(description);
}

void PendingChangeLog::record(ChangeType type, const entry::ShareSettlement& entry)
{
	std::string description = std::format("Fremdanteil-Begleichung {} - Betrag: {}, Kommentar: {}", actionVerb.at(type), entry.amount, entry.comment);

	addLogEntry(description);
}

void PendingChangeLog::record(const std::string& description)
{
	addLogEntry(description);
}

void PendingChangeLog::addLogEntry(const std::string& description)
{
	log.emplace_back(
		QDateTime::currentDateTime(), 
		description
	);
}

void PendingChangeLog::clear()
{
	log.clear();
}