#include "Exporter.h"
#include "qtutils/QtConversions.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QImage>
#include <QPainter>
#include <filesystem>
#include <fstream>
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

		enum class Theme
		{
			dark, bright
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
	
		QImage renderCellsAsImage(const std::vector<exportType::personDebt>& entries, Theme theme)
		{		
			int scale = 3;

			constexpr int rowHeight = 28;
			constexpr int colWidths[] = { 150, 100 };
			constexpr int totalWidth = colWidths[0] + colWidths[1];
			const int totalHeight = rowHeight * (static_cast<int>(entries.size()) + 1);

			QImage image(QSize(totalWidth, totalHeight) * scale, QImage::Format_ARGB32);
			image.setDevicePixelRatio(scale);
			image.fill(theme == Theme::bright ? Qt::white : Qt::black);

			QPainter painter(&image);
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setRenderHint(QPainter::TextAntialiasing);

			painter.setPen(theme == Theme::bright ? Qt::black : Qt::white);
			QFont font = painter.font();
			font.setPointSize(10);
			painter.setFont(font);

			auto drawRow = [&](int rowIndex, const QString& col1, const QString& col2, bool bold)
				{
					QFont f = painter.font();
					f.setBold(bold);
					painter.setFont(f);

					const int y = rowIndex * rowHeight;
					QRect cell1(0, y, colWidths[0], rowHeight);
					QRect cell2(colWidths[0], y, colWidths[1], rowHeight);

					painter.drawRect(cell1);
					painter.drawRect(cell2);
					painter.drawText(cell1.adjusted(6, 0, -6, 0), Qt::AlignVCenter | Qt::AlignLeft, col1);
					painter.drawText(cell2.adjusted(6, 0, -6, 0), Qt::AlignVCenter | Qt::AlignRight, col2);
				};

			drawRow(0, "Name", "Ausstand ("+ QtUtils::eurSymbol() + ")", true);

			int row = 1;
			for (const auto& entry : entries)
			{
				drawRow(row++, QString::fromStdString(entry.name), QString::fromLatin1(formatDebt(entry.debt)), false);
			}

			painter.end();
			return image;
		}
	}

	void toClipboard(const std::vector<exportType::personDebt>& entries)
	{
		Formats formats = getTextFormats(entries);
		QImage image = renderCellsAsImage(entries, Theme::dark);

		auto* data = new QMimeData();
		data->setText(QString::fromStdString(formats.tsv));
		data->setHtml(QString::fromStdString(formats.html));
		data->setData("text/csv", QByteArray::fromStdString(formats.csv));
		data->setImageData(image);

		QApplication::clipboard()->setMimeData(data);
	}

	void toCSV(const std::vector<exportType::personDebt>& entries, const std::string& savePath)
	{
		std::string csvContent = getTextFormats(entries).csv;

		std::filesystem::path path{ std::u8string(reinterpret_cast<const char8_t*>(savePath.data()), savePath.size()) }; // use UFT-8

		// create directory, if not existing
		std::filesystem::create_directories(path.parent_path());

		// creates file on open, if not existing
		std::ofstream file(path, std::ios::binary);

		if (!file.is_open());
			// handle open error

		static constexpr char utf8Bom[] = { '\xEF', '\xBB', '\xBF' }; ; // mark file as UTF-8 (BOM: EF BB BF)
		file.write(utf8Bom, sizeof(utf8Bom)); // write all three bytes of BOM to file
		file.write(csvContent.data(), static_cast<std::streamsize>(csvContent.size())); // data() returns char* from string; size() returns (unsigned) size_t, streamsize is (signed) numeric value

		if (!file.good());
			// handle write error

		file.close();

		if (!file.good());
			// handle close error
	}

	void toWeb(const std::vector<exportType::personDebt>& entries)
	{

	}
}