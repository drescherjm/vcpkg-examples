#include "MonitorWidget.h"
#include <QTimer>
#include <QScreen>
#include <QDebug>
#include <iostream>

#define UNICODE
#include <windows.h>
#include "smSystemDisplayManager.h"
#include "smSystemDisplay.h"

std::shared_ptr<smSystemDisplayManager> pDisplayManager{ std::make_shared<smSystemDisplayManager>() };

/////////////////////////////////////////////////////////////////////////////////////////

MonitorWidget::MonitorWidget(QWidget* parent /*= 0*/) : QWidget(parent)
{
	setupUi(this);

	emit pDisplayManager->refresh();

	auto pTimer = new QTimer(this);
	//pTimer->setInterval(1000);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(refreshDisplay()));
	pTimer->start(1000);
}

/////////////////////////////////////////////////////////////////////////////////////////

MonitorWidget::~MonitorWidget()
{

}

void MonitorWidget::enumerateQtScreens()
{
	QList<QScreen*> screens = QGuiApplication::screens();
	for (QScreen* screenInfo : screens)
	{
		qDebug() << "name: " << screenInfo->name();
		qDebug() << "geometry: " << screenInfo->geometry();
		qDebug() << "refreshRate: " << screenInfo->refreshRate();
		qDebug() << "manufacturer: " << screenInfo->manufacturer();
		qDebug() << "size: " << screenInfo->size().width() << screenInfo->size().height();
		qDebug() << "model: " << screenInfo->model();
		qDebug() << "serial number: " << screenInfo->serialNumber();
		qDebug() << "DPI X: " << screenInfo->physicalDotsPerInchX();
		qDebug() << "DPI Y: " << screenInfo->physicalDotsPerInchY();
		qDebug() << "\n";

	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void MonitorWidget::enumerateWindowsScreens()
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

	DWORD deviceNum = 0;
	while (EnumDisplayDevices(NULL, deviceNum, &dd, 0))
	{
		qDebug() << "display: " << QString::fromWCharArray(dd.DeviceName) << "\n" <<
			"Device String: "  << QString::fromWCharArray(dd.DeviceString) << "\n" <<
			"Device ID: " << QString::fromWCharArray(dd.DeviceID) << "\n" <<
			"Device Key: " << QString::fromWCharArray(dd.DeviceKey) << "\n";

		DISPLAY_DEVICE ddmon = { 0 };
		ddmon.cb = sizeof(DISPLAY_DEVICE);
		DWORD monitorNum = 0;
		while (EnumDisplayDevices(dd.DeviceName, monitorNum, &ddmon, 0))
		{
			qDebug() << "monitor: " << QString::fromWCharArray(ddmon.DeviceName) << "\n" <<
				"Device String: " << QString::fromWCharArray(ddmon.DeviceString) << "\n" <<
				"Device ID: " << QString::fromWCharArray(ddmon.DeviceID) << "\n" <<
				"Device Key: " << QString::fromWCharArray(ddmon.DeviceKey) << "\n";

			monitorNum++;
		}
		deviceNum++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void MonitorWidget::refreshDisplay()
{
	auto pScreen = screen();	
	if (pScreen) {
// 		if (pScreen->serialNumber().isEmpty()) {
// 			if (!pScreen->setProperty("serialNumber", QString("Test"))) {
// 				std::cerr << "Could not modify the serialNumber" << std::endl;
// 			}
// 		}
// 
// 		lineEditScreen->setText(pScreen->name());
// 		lineEditSerialNumber->setText(pScreen->serialNumber());


		smSystemDisplay display;

		display.setSystemDisplayManager(pDisplayManager);
		display.initialize(pScreen);

		lineEditName->setText(display.getName());
		lineEditID->setText(display.getMonitorIDs());
		lineEditKey->setText(display.getMonitorKeys());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
