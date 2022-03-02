#pragma once

#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QWidget>

#include "ui_MonitorWidget.h"

class MonitorWidget : public QWidget, public Ui_MonitorWidget
{
	Q_OBJECT

public:
	explicit MonitorWidget(QWidget* parent = 0);
	~MonitorWidget();

public:
	void	enumerateQtScreens();
	void	enumerateWindowsScreens();


protected slots:
	void	refreshDisplay();
};

#endif // MONITORWIDGET_H