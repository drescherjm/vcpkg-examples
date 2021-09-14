// #include <QApplication>
// #include <QFile>
// #include <QTextStream>
#include "DM0Dialog.h"

/////////////////////////////////////////////////////////////////////////////////////////

void setupDarkMode()
{
	QFile f(":qdarkstyle/style.qss");

	if (!f.exists()) {
		printf("Unable to set stylesheet, file not found\n");
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}
}


/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	setupDarkMode();

	DM0Dialog mainWindow;
	mainWindow.setGeometry(100, 100, 700, 700);
	mainWindow.show();

    return app.exec();
}

/////////////////////////////////////////////////////////////////////////////////////////
