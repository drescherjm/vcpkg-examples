#include <QApplication>
#include "QGraphicsView0.h"

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	QGraphicsView0 mainWindow;
	mainWindow.setGeometry(100, 100, 700, 700);
	mainWindow.show();

    return app.exec();
}
