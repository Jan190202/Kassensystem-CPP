#include "FinancialStateBefore.h"
#include "qtutils/ReadJson.h"

#include <expected>
#include <string>

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

namespace financialStateBefore
{
	registerFinancials::State read()
	{
		std::string fileName = "FinancialsBefore.json";

		registerFinancials::State state{};
		const auto result = readJson::getQJsonObj(fileName);
		
		if (!result.has_value())
			switch (result.error())
			{
			case readJson::Exception::MissingJsonObject:
				qDebug() << fileName << ": " << "No JSON object found";
				break;
			case readJson::Exception::OpeningFileFailed:
				qDebug() << fileName << ": " << "Failed opening file";
				break;
			case readJson::Exception::ParsingJsonFailed:
				qDebug() << fileName << ": " << "Failed parsing JSON document";
				break;

			qDebug() << "Returning default object";
			return state;
			}
			
		QJsonObject jsonObj = result.value();

		state.date			= QDate::fromString(jsonObj["date"].toString(), "dd.MM.yyyy");
		state.savings		= jsonObj["savings"].toDouble();
		state.cash			= jsonObj["cash"].toDouble();
		state.foreignCash	= jsonObj["foreignCash"].toDouble();
		state.ownCash = state.cash - state.foreignCash;
		
		qDebug() << state;
		
		return state;
	}
}