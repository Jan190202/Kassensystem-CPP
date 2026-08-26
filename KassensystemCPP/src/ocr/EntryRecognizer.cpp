#include "EntryRecognizer.h"
#include <iostream>
#include <filesystem>
#include <QCoreApplication>

static std::filesystem::path getExecutableDir()
{
	return std::filesystem::path(QCoreApplication::applicationDirPath().toStdString()); // get path of exe/dll this is running from
}

PYBIND11_EMBEDDED_MODULE(consumptionModule, m)
{
	py::class_<ConsumptionInputs>(m, "ConsumptionInputs")
		.def(py::init<>())
		.def_readwrite("personName", &ConsumptionInputs::personName)
		.def_readwrite("nBeer05", &ConsumptionInputs::nBeer05)
		.def_readwrite("nBeer04", &ConsumptionInputs::nBeer04)
		.def_readwrite("nSoftdrinks", &ConsumptionInputs::nSoftdrinks)
		.def_readwrite("nWater", &ConsumptionInputs::nWater)
		.def_readwrite("otherExpense", &ConsumptionInputs::otherExpense);
}

EntryRecognizer& EntryRecognizer::instance()
{
	static EntryRecognizer instance;
	return instance;
}

void EntryRecognizer::initInterpreter()
{
	if (isInterpreterInit) return;

	try
	{
		_putenv_s("PYTHONHOME", PYTHON_HOME_DIR); // from CMake compile definition

		pyInterpreter.emplace();

		std::filesystem::path scriptsDir = getExecutableDir() / "scripts"; // overloaded operator/ in path -> concatenate parts correctly (OS-independent)

		py::module_ sys = py::module_::import("sys");
		sys.attr("path").attr("append")(scriptsDir.string());

		isInterpreterInit = true;
	}
	catch (py::error_already_set& e)
	{
		std::cerr << "Python init failed: " << e.what() << std::endl;
		throw;
	}
}

void EntryRecognizer::shutdownInterpreter()
{
	if (isInterpreterInit)
	{
		pyInterpreter.reset();
		isInterpreterInit = false;
	}
}

std::vector<ConsumptionInputs> EntryRecognizer::readDocument(const std::string& imagePath)
{
	initInterpreter();

	try
	{
		py::module_ ocr = py::module_::import("OCR");
		py::object pyResult = ocr.attr("readDocument")(imagePath);

		if (!py::isinstance<py::list>(pyResult))
		{
			std::cerr << "OCR.readDocument() did not return a list" << std::endl;
			throw std::runtime_error("Invalid return type from Python script");
		}

		std::vector<ConsumptionInputs> result = pyResult.cast<std::vector<ConsumptionInputs>>();

		return result;
	}
	catch (py::error_already_set& e)
	{
		std::cerr << "Python error in readDocument: " << e.what() << std::endl;
		throw;
	}
}