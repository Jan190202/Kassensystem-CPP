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

	std::expected<QJsonObject, Exception> getQJsonObj(const std::string& fileName, const std::string& relPath = "data");
}