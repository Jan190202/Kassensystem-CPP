#include "gui/IconLoader.h"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QStyleHints>
#include <QString>
#include <QDir>
#include <QPixmap>
#include <QPainter>

namespace iconLoader
{
	namespace
	{
		bool isDarkMode() 
		{
			return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
		}
	}

	QIcon getIcon(const std::string& fileNameStr, bool isThemeDependent, const std::string& relPathStr)
	{
		QString fileName = QString::fromStdString(fileNameStr);
		QString relPath = QString::fromStdString(relPathStr);

		QDir exePath{ QCoreApplication::applicationDirPath() };
		QDir folderPath{ exePath.filePath(relPath) };
		QString filePath = folderPath.filePath(fileName);

		if (!QFileInfo::exists(filePath)) 
		{
			qDebug() << "Icon not found: " << fileName;
			return QIcon();
		}

		QPixmap pixmap(filePath);

		if (isThemeDependent && isDarkMode()) // change black icons to white
		{
			QPixmap recolored(pixmap.size());
			recolored.fill(Qt::transparent);

			QPainter painter(&recolored);
			painter.drawPixmap(0, 0, pixmap);
			painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			painter.fillRect(recolored.rect(), Qt::white);
			painter.end();

			return QIcon(recolored);
		}

		return QIcon(pixmap);
	}
}
