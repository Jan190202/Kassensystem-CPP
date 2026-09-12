#include "SystemConfig.h"
#include <QtGlobal>
#include <QString>
#include <Windows.h>
#include <cstdio>


namespace systemConfig
{
	namespace
	{
		void utf8MessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
		{
			QByteArray utf8Msg = msg.toUtf8();
			FILE* stream = (type == QtDebugMsg || type == QtInfoMsg) ? stdout : stderr;
			fprintf(stream, "%s\n", utf8Msg.constData());
		}
	}

	void setUTF8Encoding() // correct handling of non-ascii characters (ß,ä,ä,ü,...)
	{
		// consol setup: UTF-8 encoding for correct display output
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);

		// Qt setup: force qDebug to output QStrings in UTF-8 instead of down encoding to CP-1252
		qInstallMessageHandler(utf8MessageHandler);
	}
}