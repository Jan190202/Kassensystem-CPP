#include "ReadJson.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <string>

namespace readJson
{
	std::expected<QJsonObject,Exception> getQJsonObj(std::string fileNameStr, std::string relPath)
	{
		QString exePath = QCoreApplication::applicationDirPath();
		QString fileName = QString::fromStdString(fileNameStr);
		QString subFolderName = QString::fromStdString(relPath);
		QString filePath = exePath + "/" + subFolderName + "/" + fileName;

		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return std::unexpected(Exception::OpeningFileFailed);

		QByteArray fileContent = file.readAll();
		file.close();

		QJsonParseError parseError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);

		if (parseError.error != QJsonParseError::NoError)
			return std::unexpected(Exception::ParsingJsonFailed);

		if (!jsonDoc.isObject())
			return std::unexpected(Exception::MissingJsonObject);

		return jsonDoc.object();
	}
}