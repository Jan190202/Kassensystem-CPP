#include "JsonReader.h"

#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <string>

namespace jsonReader
{
	std::expected<QJsonObject,Exception> getQJsonObj(const std::string& fileNameStr, const std::string& relPathStr)
	{
		QString fileName = QString::fromStdString(fileNameStr);
		QString relPath = QString::fromStdString(relPathStr);

		QDir exePath{ QCoreApplication::applicationDirPath() };
		QDir folderPath{ exePath.filePath(relPath) };
		QString filePath = folderPath.filePath(fileName);

		QFile file{ filePath };
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return std::unexpected(Exception::openingFileFailed);

		QByteArray fileContent = file.readAll();
		file.close();

		QJsonParseError parseError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);

		if (parseError.error != QJsonParseError::NoError)
			return std::unexpected(Exception::parsingJsonFailed);

		if (!jsonDoc.isObject())
			return std::unexpected(Exception::missingJsonObject);

		return jsonDoc.object();
	}
}