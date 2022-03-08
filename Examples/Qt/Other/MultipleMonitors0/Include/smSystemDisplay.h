#pragma once

#ifndef SMSYSTEMDISPLAY_H
#define SMSYSTEMDISPLAY_H

/////////////////////////////////////////////////////////////////////////////////////////

class smSystemDisplayManager;

/////////////////////////////////////////////////////////////////////////////////////////

#include <QObject>
#include <QScreen>
#include <memory>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////

struct smSystemDisplayInfo
{
	QString m_strDeviceName;
	QString m_strDeviceString;
	QString	m_strDisplayID;
	QString m_strDisplayKey;
	uint32_t m_nDisplayState;

	struct smSystemMontorInfo {
		QString m_strMonitorName;
		QString m_strMonitorString;
		QString m_strMonitorID;
		QString m_strMonitorKey;
		uint32_t m_nMonitorState;
	};

	std::vector<smSystemMontorInfo> m_vecMonitors;
};

/////////////////////////////////////////////////////////////////////////////////////////

class smSystemDisplay : public QObject
{
	Q_OBJECT
public:
	explicit smSystemDisplay(QObject* parent=nullptr);
	~smSystemDisplay();
public:
	void	initialize(QScreen* pScreen);
	void	setSystemDisplayManager(std::shared_ptr<smSystemDisplayManager>& pDisplayMgr);
public:
	QString		getName() const;
	QString		getMonitorKeys() const;
	QString		getMonitorIDs() const;

public:
	void	setInfo(const smSystemDisplayInfo& info);

public slots:
	void		refresh();

private:
	class smPrivate;
	std::unique_ptr<smPrivate> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMSYSTEMDISPLAY_H
