#ifndef QtVTKRenderWindow_H
#define QtVTKRenderWindow_H

#include "vtkSmartPointer.h"
#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
#include "vtkResliceImageViewerMeasurements.h"
#include <QMainWindow>

// Forward Qt class declarations
class Ui_QtVTKRenderWindow;

class QtVTKRenderWindow : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  QtVTKRenderWindow(int argc, char *argv[]);
  ~QtVTKRenderWindow() override {}

public slots:

  virtual void slotExit();
  virtual void resliceMode(int);
  virtual void thickMode(int);
  virtual void SetBlendModeToMaxIP();
  virtual void SetBlendModeToMinIP();
  virtual void SetBlendModeToMeanIP();
  virtual void SetBlendMode(int);
  virtual void ResetViews();
  virtual void Render();

protected:
  vtkSmartPointer< vtkResliceImageViewer > riw;
  vtkSmartPointer< vtkDistanceWidget > DistanceWidget[3];
  vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;

protected slots:

private:

  // Designer form
  Ui_QtVTKRenderWindow *ui;
};

#endif // QtVTKRenderWindow_H
