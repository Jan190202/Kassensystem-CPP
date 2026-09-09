#include "PriceList.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

namespace priceList
{
	Entries read()
	{
		Entries entries;
		
		QString exePath = QCoreApplication::applicationDirPath();
		QString fileName = QStringLiteral("PriceList.json");
		QString subFolderName = QStringLiteral("data");
		QString filePath = exePath + "/" + subFolderName + "/" + fileName;

		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Can't open priceList.json" << "\n"
						<< "Using default price list";
			return entries;
		}

		QByteArray fileContent = file.readAll();
		file.close();

		QJsonParseError parseError;
		QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent, &parseError);

		if (parseError.error != QJsonParseError::NoError) 
		{
			qCritical() << "JSON parse error in price list:" << parseError.errorString() << "\n"
						<< "Using default price list";
			return entries;
		}

		if (jsonDoc.isObject())
		{
			QJsonObject jsonObj = jsonDoc.object();

			entries.beer05		= jsonObj["Bier_0.5l"].toDouble();
			entries.beer04		= jsonObj["Bier_0.4l"].toDouble();
			entries.softdrink	= jsonObj["Softdrink"].toDouble();
			entries.water		= jsonObj["Wasser"].toDouble();
		}

		//qDebug() << entries;

		return entries;
	}
}