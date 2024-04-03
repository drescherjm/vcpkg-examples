#include "smSystemDisplay.h"
#include "smSystemDisplayManager.h"

/////////////////////////////////////////////////////////////////////////////////////////

class smSystemDisplay::smPrivate
{
public:
	std::shared_ptr<smSystemDisplayManager> m_pDisplayManager;
	QString									m_strDisplayName;
	QString									m_strExtendedKey;
	smSystemDisplayData						m_info;
};

/////////////////////////////////////////////////////////////////////////////////////////

smSystemDisplay::smSystemDisplay(QObject* parent) : m_pPrivate{std::make_unique<smPrivate>()}
{

}

/////////////////////////////////////////////////////////////////////////////////////////

smSystemDisplay::~smSystemDisplay()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

void smSystemDisplay::initialize(QScreen* pScreen)
{
	if (pScreen) {
		m_pPrivate->m_strDisplayName = pScreen->name();
		emit refresh();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smSystemDisplay::setSystemDisplayManager(std::shared_ptr<smSystemDisplayManager>& pDisplayMgr)
{
	m_pPrivate->m_pDisplayManager = pDisplayMgr;
}

/////////////////////////////////////////////////////////////////////////////////////////

QString smSystemDisplay::getName() const
{
	return m_pPrivate->m_strDisplayName;
}

/////////////////////////////////////////////////////////////////////////////////////////

QString smSystemDisplay::getMonitorKeys() const
{
	QString retVal;

	if (!m_pPrivate->m_info.m_vecMonitors.empty()) {
		size_t i{};
		for (auto& info : m_pPrivate->m_info.m_vecMonitors) {
			if (i > 1) {
				retVal += ';';
			}
			retVal += info.m_strMonitorKey;
			i++;
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

QString smSystemDisplay::getMonitorIDs() const
{
	QString retVal;

	if (!m_pPrivate->m_info.m_vecMonitors.empty()) {
		size_t i{};
		for (auto& info : m_pPrivate->m_info.m_vecMonitors) {
			if (i > 1) {
				retVal += ';';
			}
			retVal += info.m_strMonitorID;
			i++;
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smSystemDisplay::setInfo(const smSystemDisplayData& info)
{
	m_pPrivate->m_info = info;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smSystemDisplay::refresh()
{
	if (m_pPrivate->m_pDisplayManager && !m_pPrivate->m_strDisplayName.isEmpty()) {
		m_pPrivate->m_pDisplayManager->search(this);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
