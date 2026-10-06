#include "domain/model/Entities.h"
#include "common/Utils.h"
#include <format>

namespace
{
	using ChangeType = PendingChangeLog::ChangeType;
	using Change = PendingChangeLog::Change;
	using Field = PendingChangeLog::Field;

	constexpr const char* colorAdd = "#2e9e5b";
	constexpr const char* colorMutate = "#d99a00";
	constexpr const char* colorRemove = "#d64545";
	constexpr const char* colorMuted = "#8a8a8a";

	const char* verb(ChangeType type)
	{
		switch (type)
		{
		case ChangeType::add:    return "hinzugefügt";
		case ChangeType::remove: return "gelöscht";
		case ChangeType::mutate: return "verändert";
		}
		return "";
	}

	const char* color(ChangeType type)
	{
		switch (type)
		{
		case ChangeType::add:    return colorAdd;
		case ChangeType::remove: return colorRemove;
		case ChangeType::mutate: return colorMutate;
		}
		return colorMuted;
	}

	const char* symbol(ChangeType type)
	{
		switch (type)
		{
		case ChangeType::add:    return "+";
		case ChangeType::remove: return "−";
		case ChangeType::mutate: return "~";
		}
		return "";
	}

	std::string escapeHtml(const std::string& text)
	{
		return QString::fromStdString(text).toHtmlEscaped().toStdString();
	}

	// "Label: value, Label: "quoted value", ..."
	std::string plainFields(const Change& change)
	{
		std::string out;

		for (size_t i = 0; i < change.fields.size(); i++)
		{
			const Field& field = change.fields[i];

			if (i != 0) out += ", ";
			out += field.quoted
				? std::format("{}: \"{}\"", field.label, field.value)
				: std::format("{}: {}", field.label, field.value);
		}

		return out;
	}

	// "<b>Label:</b> value · <b>Label:</b> <i>"quoted value"</i>"
	std::string richFields(const Change& change)
	{
		std::string out;

		for (size_t i = 0; i < change.fields.size(); i++)
		{
			const Field& field = change.fields[i];

			if (i != 0) out += std::format(" &nbsp;<span style=\"color:{};\">·</span>&nbsp; ", colorMuted);

			out += field.quoted
				? std::format("<b>{}:</b> <i>&quot;{}&quot;</i>", escapeHtml(field.label), escapeHtml(field.value))
				: std::format("<b>{}:</b> {}", escapeHtml(field.label), escapeHtml(field.value));
		}

		return out;
	}

	std::string plainRow(size_t number, const Change& change)
	{
		return std::format("{}. {}", number, change.plainText());
	}

	std::string richRow(size_t number, const Change& change)
	{
		const std::string time = escapeHtml(change.time.toString("HH:mm:ss").toStdString());

		std::string row = "<tr>";
		row += std::format("<td align=\"right\" style=\"color:{};\">{}.</td>", colorMuted, number);
		row += std::format("<td style=\"color:{};\">{}</td>", colorMuted, time);

		if (!change.isFreeText())
		{
			const ChangeType type = change.type.value();

			row += std::format(
				"<td style=\"white-space:nowrap;\"><span style=\"color:{}; font-weight:bold;\">{} {} {}</span></td>",
				color(type), symbol(type), escapeHtml(change.subject), verb(type));
			row += std::format("<td>{}</td>", richFields(change));
		}
		else if (change.type.has_value())
		{
			const ChangeType type = change.type.value();

			row += std::format(
				"<td colspan=\"2\" style=\"color:{};\"><b>{}</b> {}</td>",
				color(type), symbol(type), escapeHtml(change.message));
		}
		else
		{
			row += std::format("<td colspan=\"2\" style=\"color:{};\"><i>{}</i></td>", colorMuted, escapeHtml(change.message));
		}

		row += "</tr>";
		return row;
	}
}

std::string PendingChangeLog::Change::plainText() const
{
	if (isFreeText())
		return message;

	std::string out = std::format("{} {}", subject, verb(type.value()));

	if (!fields.empty())
		out += " - " + plainFields(*this);

	return out;
}

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
	addLogEntry(type, "Bilanz", 
		{
			{ "Beschreibung", entry.description, true },
			{ "Betrag", utils::toCurrencyFormat(entry.amount) },
			{ "ausgelegt von", entry.person.has_value() ? entry.person.value().toString() : "niemand" }
		});
}

void PendingChangeLog::record(ChangeType type, const entry::Debt& dEntry, const std::optional<entry::Consumption>& cEntry)
{
	std::vector<Field> fields = 
	{
		{ "Person", dEntry.person.toString() },
		{ "Betrag", utils::toCurrencyFormat(dEntry.amount) }
	};

	if (cEntry.has_value())
	{
		const entry::Consumption& c = cEntry.value();

		fields.push_back({ "Mengen", std::format("({}x, {}x, {}x, {}x, {})",
			c.nBeer05, c.nBeer04, c.nSoftdrinks, c.nWater, utils::toCurrencyFormat(c.otherExpense)) });
	}

	addLogEntry(type, "Schulden", std::move(fields));
}

void PendingChangeLog::record(ChangeType type, const entry::Credit& entry)
{
	addLogEntry(type, "Guthaben", 
		{
			{ "Person", entry.person.toString() },
			{ "Betrag", utils::toCurrencyFormat(entry.amount) }
		});
}

void PendingChangeLog::record(ChangeType type, const entry::Payment& entry)
{
	addLogEntry(type, "Zahlung", 
		{
			{ "Person", entry.person.toString() },
			{ "Betrag", utils::toCurrencyFormat(entry.amount) },
			{ "Überlauf", entry.overpaymentType == OverpaymentDisposition::credit ? "Guthaben" : "Trinkgeld" }
		});
}

void PendingChangeLog::record(ChangeType type, const entry::Person& entry)
{
	addLogEntry(type, "Person", 
		{
			{ "Bezeichner", entry.getFullSpecifier(), true }
		});
}

void PendingChangeLog::record(ChangeType type, const entry::ShareSettlement& entry)
{
	addLogEntry(type, "Fremdanteil-Begleichung", 
		{
			{ "Betrag", utils::toCurrencyFormat(entry.amount) },
			{ "Kommentar", entry.comment, true }
		});
}

void PendingChangeLog::record(const std::string& description, std::optional<ChangeType> type)
{
	log.push_back(Change{ QDateTime::currentDateTime(), type, {}, {}, description });
}

void PendingChangeLog::addLogEntry(ChangeType type, std::string subject, std::vector<Field> fields)
{
	log.push_back(Change{ QDateTime::currentDateTime(), type, std::move(subject), std::move(fields), {} });
}

void PendingChangeLog::clear()
{
	log.clear();
}

std::string PendingChangeLog::printPendingChanges(TextFormat format) const
{
	if (format == TextFormat::rich)
	{
		std::string out = "<table cellspacing=\"0\" cellpadding=\"3\">";

		for (size_t i = 0; i < log.size(); i++)
			out += richRow(i + 1, log[i]);

		out += "</table>";
		return out;
	}

	std::string out;

	for (size_t i = 0; i < log.size(); i++)
	{
		if (i != 0) out += "\n";
		out += plainRow(i + 1, log[i]);
	}

	return out;
}