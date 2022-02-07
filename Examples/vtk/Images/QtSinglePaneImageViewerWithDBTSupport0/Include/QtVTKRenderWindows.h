#ifndef QtVTKRenderWindows_H
#define QtVTKRenderWindows_H

#include "vtkSmartPointer.h"
#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
#include "vtkResliceImageViewerMeasurements.h"
#include <QMainWindow>
#include <string>

class vtkImageData;

using VTKViewWidget = vtkResliceImageViewer;
//using VTKViewWidget = vtkImageViewer2;


// Forward Qt class declarations
class Ui_QtVTKRenderWindow;

class QtVTKRenderWindows : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  QtVTKRenderWindows(int argc, char *argv[]);
  ~QtVTKRenderWindows() override {}

public slots:
	//virtual void setupTimer(int nCineDelay);
	virtual void initializeDisplay(std::string strFileName, int nCineDelay);
	virtual void slotExit();
	virtual void resliceMode(int);
	virtual void thickMode(int);
	virtual void SetBlendModeToMaxIP();
	virtual void SetBlendModeToMinIP();
	virtual void SetBlendModeToMeanIP();
	virtual void SetBlendMode(int);
	virtual void ResetViews();
	virtual void Render();
	virtual void AddDistanceMeasurementToView1();
  //virtual void AddDistanceMeasurementToView( int );

protected:
  vtkSmartPointer< VTKViewWidget > riw;
  vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;

protected slots:
	void nextSlice();

private:
	void setupCamera(vtkImageData* pImage);

private:

  // Designer form
  Ui_QtVTKRenderWindow *ui;
};

#endif // QtVTKRenderWindows_H
