#include "PriceListLoader.h"
#include "JsonReader.h"

#include <expected>
#include <string>

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


namespace priceListLoader
{
	PriceList read()
	{
		std::string fileName = "PriceList.json";

		PriceList priceList;
		const auto result = jsonReader::getQJsonObj(fileName);

		if (!result.has_value())
			switch (result.error())
			{
			case jsonReader::Exception::MissingJsonObject:
				qDebug() << fileName << ": " << "No JSON object found";
				break;
			case jsonReader::Exception::OpeningFileFailed:
				qDebug() << fileName << ": " << "Failed opening file";
				break;
			case jsonReader::Exception::ParsingJsonFailed:
				qDebug() << fileName << ": " << "Failed parsing JSON document";
				break;

				qDebug() << "Returning default object";
				return priceList;
			}

		QJsonObject jsonObj = result.value();

		priceList.beer05 = jsonObj["Bier_0.5l"].toDouble();
		priceList.beer04 = jsonObj["Bier_0.4l"].toDouble();
		priceList.softdrink = jsonObj["Softdrink"].toDouble();
		priceList.water = jsonObj["Wasser"].toDouble();

		qDebug() << priceList;

		return priceList;
	}
}