#include "smSystemDisplayManager.h"
#include "smSystemDisplay.h"
#include <QGuiApplication>

#define UNICODE
#include <windows.h>

/////////////////////////////////////////////////////////////////////////////////////////

class smSystemDisplayManager::smPrivate
{
public:
	using DisplayMap = std::map<std::string, smSystemDisplayData>;
public:
	DisplayMap m_Map;
};

/////////////////////////////////////////////////////////////////////////////////////////

smSystemDisplayManager::smSystemDisplayManager(QObject* parent) : m_pPrivate{std::make_unique<smPrivate>()}
{
	auto pGUIApp = qGuiApp;
	if (pGUIApp) {
		connect(pGUIApp, SIGNAL(screenAdded(QScreen*)), this, SLOT(refresh()));
		connect(pGUIApp, SIGNAL(screenRemoved(QScreen*)), this, SLOT(refresh()));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smSystemDisplayManager::search(smSystemDisplay* pDisplay)
{
	if (pDisplay) {
		auto it = m_pPrivate->m_Map.find(pDisplay->getName().toStdString());
		if (it != m_pPrivate->m_Map.end()) {
			pDisplay->setInfo(it->second);
		}
		else {
// 			emit refresh();
// 			it = m_pPrivate->m_Map.find(pDisplay->getName().toStdString());
// 			if (it != m_pPrivate->m_Map.end()) {
// 				pDisplay->setInfo(it->second);
// 			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smSystemDisplayManager::refresh()
{
	m_pPrivate->m_Map.clear();

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

	DWORD deviceNum = 0;
	while (EnumDisplayDevices(NULL, deviceNum, &dd, 0))
	{
		smSystemDisplayData info;

		info.m_strDeviceName = QString::fromWCharArray(dd.DeviceName);
		info.m_strDeviceString = QString::fromWCharArray(dd.DeviceString);
		info.m_nDisplayState = dd.StateFlags;
		info.m_strDisplayKey = QString::fromWCharArray(dd.DeviceKey);
		info.m_strDisplayID = QString::fromWCharArray(dd.DeviceID);

		DISPLAY_DEVICE ddmon = { 0 };
		ddmon.cb = sizeof(DISPLAY_DEVICE);
		DWORD monitorNum = 0;
		while (EnumDisplayDevices(dd.DeviceName, monitorNum, &ddmon, 0))
		{
			smSystemDisplayData::smSystemMontorInfo monInfo;
			monInfo.m_strMonitorName = QString::fromWCharArray(ddmon.DeviceName);
			monInfo.m_strMonitorString = QString::fromWCharArray(ddmon.DeviceString);
			monInfo.m_nMonitorState = ddmon.StateFlags;
			monInfo.m_strMonitorKey = QString::fromWCharArray(ddmon.DeviceKey);
			monInfo.m_strMonitorID = QString::fromWCharArray(ddmon.DeviceID);

			info.m_vecMonitors.push_back(monInfo);

			monitorNum++;
		}
		deviceNum++;

		m_pPrivate->m_Map[info.m_strDeviceName.toStdString()] = info;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////