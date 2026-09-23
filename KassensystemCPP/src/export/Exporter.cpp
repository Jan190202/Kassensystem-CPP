#include "Exporter.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <filesystem>
#include <tuple>
#include <format>

namespace fs = std::filesystem;

namespace exporter
{
	namespace
	{
		struct Formats
		{
			std::string html, csv, tsv;
		};

		// csv field: wrap in quotes if it contains a comma, quote, or newline; double any internal quotes.
		std::string csvEscape(const std::string& field)
		{
			bool needsQuoting = field.find_first_of(",\"\r\n") != std::string::npos;
			if (!needsQuoting)
				return field;

			std::string escaped = "\"";
			for (char c : field)
			{
				if (c == '"')
					escaped += "\"\"";
				else
					escaped += c;
			}
			escaped += "\"";
			return escaped;
		}

		// TSV field: strip characters that would break the tab/row structure
		std::string tsvSanitize(const std::string& field)
		{
			std::string out = field;
			for (char& c : out)
			{
				if (c == '\t' || c == '\n' || c == '\r')
					c = ' ';
			}
			return out;
		}

		std::string htmlEscape(const std::string& field)
		{
			std::string out;
			out.reserve(field.size());
			for (char c : field)
			{
				switch (c)
				{
				case '&':  out += "&amp;";  break;
				case '<':  out += "&lt;";   break;
				case '>':  out += "&gt;";   break;
				case '"':  out += "&quot;"; break;
				default:   out += c;        break;
				}
			}
			return out;
		}

		std::string formatDebt(double debt)
		{
			return std::format("{:.2f}", debt);
		}

		Formats getTextFormats(const std::vector<exportType::personDebt>& entries)
		{
			Formats formats;

			const std::string headerName = "Name";
			const std::string headerDebt = "Ausstand";

			std::ostringstream html, csv, tsv;

			// header row
			tsv << headerName << '\t' << headerDebt << "\r\n";
			csv << csvEscape(headerName) << ',' << csvEscape(headerDebt) << "\r\n";

			html << "<html><body><table border=\"1\" cellspacing=\"0\">"
				<< "<tr><td><b>" << headerName << "</b></td>"
				<< "<td><b>" << headerDebt << "</b></td></tr>";

			for (const auto& entry : entries)
			{
				const std::string debtStr = formatDebt(entry.debt);

				// TSV
				tsv << tsvSanitize(entry.name) << '\t'
					<< tsvSanitize(debtStr) << "\r\n";

				// CSV
				csv << csvEscape(entry.name) << ','
					<< csvEscape(debtStr) << "\r\n";

				// HTML
				html << "<tr><td>" << htmlEscape(entry.name) << "</td>"
					<< "<td>" << htmlEscape(debtStr) << "</td></tr>";
			}

			html << "</table></body></html>";

			formats.tsv = tsv.str();
			formats.csv = csv.str();
			formats.html = html.str();

			return formats;
		}
	}

	void toClipboard(const std::vector<exportType::personDebt>& entries)
	{
		Formats formats = getTextFormats(entries);

		auto* data = new QMimeData();
		data->setText(QString::fromStdString(formats.tsv));
		data->setHtml(QString::fromStdString(formats.html));
		data->setData("text/csv", QByteArray::fromStdString(formats.csv));

		QApplication::clipboard()->setMimeData(data);
	}

	void toCSV(const std::vector<exportType::personDebt>& entries, const std::string& savePath)
	{

	}

	void toWeb(const std::vector<exportType::personDebt>& entries)
	{

	}
}