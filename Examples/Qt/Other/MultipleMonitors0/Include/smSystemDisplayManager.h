#pragma once

#ifndef SMSYSTEMDISPLAYMANAGER_H
#define SMSYSTEMDISPLAYMANAGER_H

class smSystemDisplay;

/////////////////////////////////////////////////////////////////////////////////////////

#include <QObject>
#include <memory>

/////////////////////////////////////////////////////////////////////////////////////////

class smSystemDisplayManager : public QObject
{
	Q_OBJECT
public:
	smSystemDisplayManager(QObject* parent=nullptr);
public:
	void	search(smSystemDisplay* pDisplay);
public slots:
	void	refresh();
private:
	class smPrivate;
	std::unique_ptr<smPrivate> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMSYSTEMDISPLAYMANAGER_H
