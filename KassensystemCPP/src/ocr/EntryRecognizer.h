#pragma once

#include "gui/GuiTypes.h"

#include <vector>
#include <string>
#include <optional>

#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;

class EntryRecognizer
{
public:
	static EntryRecognizer& instance();

	EntryRecognizer(const EntryRecognizer&) = delete;
	EntryRecognizer& operator=(const EntryRecognizer&) = delete;

	std::vector<ConsumptionInputs> readDocument(const std::string& imagePath);
	void shutdownInterpreter();
private:
	EntryRecognizer() = default;
	~EntryRecognizer() = default;

	void initInterpreter();
	std::optional<py::scoped_interpreter> pyInterpreter;
	bool isInterpreterInit = false;
};
