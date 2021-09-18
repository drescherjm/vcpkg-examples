#include <QApplication>
#include <QSurfaceFormat>

#include "QVTKOpenGLWidget.h"
#include "QtVTKRenderWindow.h"

int main( int argc, char** argv )
{
  vtkObject::GlobalWarningDisplayOff();

  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

  // QT Stuff
  QApplication app( argc, argv );

  QtVTKRenderWindow myQtVTKRenderWindows(argc, argv);
  myQtVTKRenderWindows.show();

  return app.exec();
}
