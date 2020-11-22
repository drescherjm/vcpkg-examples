#include <QApplication>
#include "DM0Dialog.h"

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	DM0Dialog mainWindow;
	mainWindow.setGeometry(100, 100, 700, 700);
	mainWindow.show();

    return app.exec();
}
