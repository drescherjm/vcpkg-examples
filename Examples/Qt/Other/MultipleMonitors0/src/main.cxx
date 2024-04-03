#include <QApplication>
#include <iostream>
#include "MonitorWidget.h"

/////////////////////////////////////////////////////////////////////////////////////////

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

void myMessageHandler(QtMsgType type, const char* msg)
{
	//in this function, you can write the message to any stream!
	switch (type) {
	case QtDebugMsg:
		QLOG_DEBUG() << qPrintable(QString("qDebug: %1").arg(msg));
		break;
	case QtWarningMsg:
		QLOG_WARN() << qPrintable(QString("qWarning: %1").arg(msg));
		break;
	case QtCriticalMsg:
		QLOG_CRIT() << qPrintable(QString("qCritical: %1").arg(msg));
		break;
	case QtFatalMsg:
		QLOG_FATAL() << qPrintable(QString("qFatal: %1").arg(msg));
		abort();
	}
}

#else

void myMessageHandler(QtMsgType type, const QMessageLogContext& logContext, const QString& msg)
{
	Q_UNUSED(logContext);

	//in this function, you can write the message to any stream!
	switch (type) {
	case QtDebugMsg:
		std::cout << qPrintable(QString("qDebug: %1").arg(msg)) << std::endl;
		break;
	case QtWarningMsg:
		std::cout << qPrintable(QString("qWarning: %1").arg(msg)) << std::endl;
		break;
	case QtCriticalMsg:
		std::cout << qPrintable(QString("qCritical: %1").arg(msg)) << std::endl;
		break;
	case QtFatalMsg:
		std::cout << qPrintable(QString("qFatal: %1").arg(msg)) << std::endl;
		abort();
	}
}

#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

#ifdef _DEBUG
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)   
	qInstallMsgHandler(myMessageHandler);
#else
	qInstallMessageHandler(myMessageHandler);
#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#endif

	
	qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", QByteArray("1"));

	
	QApplication app(argc, argv);


	//app.setAttribute(Qt::AA_EnableHighDpiScaling);

	MonitorWidget mainWindow;

// 	mainWindow.enumerateQtScreens();
// 	mainWindow.enumerateWindowsScreens();

	mainWindow.show();

    return app.exec();
}

/////////////////////////////////////////////////////////////////////////////////////////
