#pragma once

class QJsonObject;
#include <string>
#include <expected>

namespace jsonReader
{
	enum class Exception
	{
		openingFileFailed, parsingJsonFailed, missingJsonObject
	};

	std::expected<QJsonObject, Exception> getQJsonObj(std::string fileName, std::string relPath = "data");
}