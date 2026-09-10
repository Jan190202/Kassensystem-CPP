#pragma once

class QJsonObject;
#include <string>
#include <expected>

namespace readJson
{
	enum class Exception
	{
		OpeningFileFailed, ParsingJsonFailed, MissingJsonObject
	};

	std::expected<QJsonObject, Exception> getQJsonObj(std::string fileName, std::string relPath = "data");
}