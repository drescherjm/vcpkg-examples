#include "ui_QtVTKRenderWindow.h"
#include "QtVTKRenderWindows.h"
#include "vtkResliceCursorCallback.h"

#include "vtkBoundedPlanePointPlacer.h"
#include "vtkCellPicker.h"
#include "vtkCommand.h"
#include "vtkDICOMImageReader.h"
#include "vtkDistanceRepresentation.h"
#include "vtkDistanceRepresentation2D.h"
#include "vtkDistanceWidget.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include "vtkHandleRepresentation.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageSlabReslice.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkProperty.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkResliceImageViewerMeasurements.h"
#include <vtkPointHandleRepresentation2D.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkProperty2D.h>
#include "dicom/vtkDICOMReader.h"
#include <QTimer>

#define TEST_SEED_WIDGET

#include <QTime>

class FunctionProfiler
{
public:
	FunctionProfiler(const QString& func) {
		m_function = QMetaObject::normalizedSignature(qPrintable(func));
		m_function = m_function.remove("__thiscall");
		m_function = m_function.remove("__stdcall");
		m_function = m_function.remove("__cdecl");

		m_startTime = QTime::currentTime();
	}

	~FunctionProfiler() {
		qint32 msecs = m_startTime.msecsTo(QTime::currentTime());
		qDebug("PROFILE: (%5d) %s", msecs, qPrintable(m_function));
	}

private:
	QString m_function;
	QTime m_startTime;
};

#define PROFILE_THIS_FUNCTION FunctionProfiler functionProfiler(Q_FUNC_INFO);

/////////////////////////////////////////////////////////////////////////////////////////

void timedRender(vtkResliceImageViewer* pView)
{
	PROFILE_THIS_FUNCTION;
	pView->Render();
}

/////////////////////////////////////////////////////////////////////////////////////////

void timedSetSlice(vtkResliceImageViewer* pView,int nSlice)
{
	PROFILE_THIS_FUNCTION;
	pView->SetSlice(nSlice);
}

/////////////////////////////////////////////////////////////////////////////////////////

void timedLoad(vtkDICOMReader* pReader)
{
	PROFILE_THIS_FUNCTION;
	pReader->Update();
}

/////////////////////////////////////////////////////////////////////////////////////////

void timedSetinputData(vtkDICOMReader* pReader,vtkResliceImageViewer* pView)
{
	PROFILE_THIS_FUNCTION;
	pView->SetInputData(pReader->GetOutput());
}

/////////////////////////////////////////////////////////////////////////////////////////

QtVTKRenderWindows::QtVTKRenderWindows(int vtkNotUsed(argc), char* argv[])
{
	this->ui = new Ui_QtVTKRenderWindow;
	this->ui->setupUi(this);

	QSize szQtWindow = size();

	vtkSmartPointer< vtkDICOMReader > reader = vtkSmartPointer< vtkDICOMReader >::New();
	reader->SetFileName(argv[1]);
	reader->SetMemoryRowOrderToFileNative();
	//reader->Update();

	timedLoad(reader);

	reader->Print(std::cout);

	int imageDims[3];
	reader->GetOutput()->GetDimensions(imageDims);

	riw = vtkSmartPointer< vtkResliceImageViewer >::New();
	vtkRenderWindow* renderWindow = vtkGenericOpenGLRenderWindow::New();

	auto renderSize = renderWindow->GetSize();

	renderSize[0] = std::max(szQtWindow.width(), renderSize[0]);
	renderSize[1] = std::max(szQtWindow.height(), renderSize[0]);

	renderWindow->SetSize(renderSize);

	riw->SetSliceOrientation(vtkResliceImageViewer::SLICE_ORIENTATION_XY);

	vtkRenderer* pren = vtkRenderer::New();
	renderWindow->AddRenderer(pren);
	riw->SetRenderWindow(renderWindow);
	riw->SetRenderer(pren);

	this->ui->view->show();

	renderSize = riw->GetSize();

	renderSize[0] = std::max(szQtWindow.width(), renderSize[0]);
	renderSize[1] = std::max(szQtWindow.height(), renderSize[0]);

	riw->SetSize(renderSize);
		
	this->ui->view->SetRenderWindow(riw->GetRenderWindow());
	riw->SetupInteractor(this->ui->view->GetRenderWindow()->GetInteractor());

	//riw->SetInputData(reader->GetOutput());

	timedSetinputData(reader, riw);

	riw->SetColorLevel(512.0);
	riw->SetColorWindow(512.0);
	
	riw->GetRenderer()->ResetCamera();

	timedRender(riw);

#ifdef TEST_SEED_WIDGET
	// Create the representation for the seed widget and for its handles
	auto handleRep = vtkPointHandleRepresentation2D::New();
	handleRep->GetProperty()->SetColor(1, 0, 0); // Make the handles red
	auto widgetRep = vtkSmartPointer<vtkSeedRepresentation>::New();
	widgetRep->SetHandleRepresentation(handleRep);

	// Create the seed widget
	auto seedWidget = vtkSeedWidget::New();

	auto pInteractor = riw->GetInteractor();
	if (pInteractor) {
		seedWidget->SetInteractor(pInteractor);
		seedWidget->SetRepresentation(widgetRep);
	}

	seedWidget->On();

#endif

	// Set up action signals and slots
	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	connect(this->ui->resliceModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(resliceMode(int)));
	connect(this->ui->thickModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(thickMode(int)));
	this->ui->thickModeCheckBox->setEnabled(0);

	connect(this->ui->radioButton_Max, SIGNAL(pressed()), this, SLOT(SetBlendModeToMaxIP()));
	connect(this->ui->radioButton_Min, SIGNAL(pressed()), this, SLOT(SetBlendModeToMinIP()));
	connect(this->ui->radioButton_Mean, SIGNAL(pressed()), this, SLOT(SetBlendModeToMeanIP()));
	this->ui->blendModeGroupBox->setEnabled(0);

	connect(this->ui->resetButton, SIGNAL(pressed()), this, SLOT(ResetViews()));
	connect(this->ui->AddDistance1Button, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToView1()));

	auto nMin = riw->GetSliceMin();
	auto nMax = riw->GetSliceMax();

	renderSize = riw->GetSize();

	riw->SetSlice((nMax - nMin) / 2);

	QTimer* timer = new QTimer(this);
	timer->setInterval(200);
	connect(timer, &QTimer::timeout, this, &QtVTKRenderWindows::nextSlice);
	timer->start();

};

void QtVTKRenderWindows::slotExit()
{
  qApp->exit();
}

void QtVTKRenderWindows::resliceMode(int mode)
{
  this->ui->thickModeCheckBox->setEnabled(mode ? 1 : 0);
  this->ui->blendModeGroupBox->setEnabled(mode ? 1 : 0);

//   for (int i = 0; i < 3; i++)
//   {
//     //riw[i]->SetResliceMode(mode ? 1 : 0);
//     riw[i]->GetRenderer()->ResetCamera();
//     riw[i]->Render();
//   }
}

void QtVTKRenderWindows::thickMode(int mode)
{
//   for (int i = 0; i < 3; i++)
//   {
//     //riw[i]->SetThickMode(mode ? 1 : 0);
//     riw[i]->Render();
//   }
}

void QtVTKRenderWindows::SetBlendMode(int m)
{
//   for (int i = 0; i < 3; i++)
//   {
//     vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(
//         vtkResliceCursorThickLineRepresentation::SafeDownCast(
//           riw[i]->GetResliceCursorWidget()->GetRepresentation())->GetReslice());
//     thickSlabReslice->SetBlendMode(m);
//     riw[i]->Render();
//   }
}

void QtVTKRenderWindows::SetBlendModeToMaxIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MAX);
}

void QtVTKRenderWindows::SetBlendModeToMinIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MIN);
}

void QtVTKRenderWindows::SetBlendModeToMeanIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MEAN);
}

void QtVTKRenderWindows::ResetViews()
{
  // Reset the reslice image views

  riw->Reset();

  // Also sync the Image plane widget on the 3D top right view with any
  // changes to the reslice cursor.
//   for (int i = 0; i < 3; i++)
//   {
//     vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
//         planeWidget[i]->GetPolyDataAlgorithm());
//     ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
//     ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
// 
//     // If the reslice plane has modified, update it on the 3D widget
//     this->planeWidget[i]->UpdatePlacement();
//   }

  // Render in response to changes.
  this->Render();
}

void QtVTKRenderWindows::Render()
{
//   for (int i = 0; i < 3; i++)
//   {
//     riw[i]->Render();
//   }
//   this->ui->view3->GetRenderWindow()->Render();

	riw->Render();
	this->ui->view->GetRenderWindow()->Render();
}

void QtVTKRenderWindows::AddDistanceMeasurementToView1()
{
 // this->AddDistanceMeasurementToView(1);
}

void QtVTKRenderWindows::nextSlice()
{
	static int diff = 1;

	auto nMin = riw->GetSliceMin();
	auto nMax = riw->GetSliceMax();
	auto nCur = riw->GetSlice();
	nCur += diff;

	if (nCur > nMax) {
		nCur = nMax;
		diff = -1;
	}

	if (nCur < nMin) {
		nCur = nMin;
		diff = 1;
	}

	timedSetSlice(riw, nCur);
}

// void QtVTKRenderWindows::AddDistanceMeasurementToView(int i)
// {
//   // remove existing widgets.
//   if (this->DistanceWidget[i])
//   {
//     this->DistanceWidget[i]->SetEnabled(0);
//     this->DistanceWidget[i] = nullptr;
//   }
// 
//   // add new widget
//   this->DistanceWidget[i] = vtkSmartPointer< vtkDistanceWidget >::New();
//   this->DistanceWidget[i]->SetInteractor(
//     this->riw[i]->GetResliceCursorWidget()->GetInteractor());
// 
//   // Set a priority higher than our reslice cursor widget
//   this->DistanceWidget[i]->SetPriority(
//     this->riw[i]->GetResliceCursorWidget()->GetPriority() + 0.01);
// 
//   vtkSmartPointer< vtkPointHandleRepresentation2D > handleRep = vtkSmartPointer< vtkPointHandleRepresentation2D >::New();
//   vtkSmartPointer< vtkDistanceRepresentation2D > distanceRep =  vtkSmartPointer< vtkDistanceRepresentation2D >::New();
// 
//   distanceRep->SetHandleRepresentation(handleRep);
//   this->DistanceWidget[i]->SetRepresentation(distanceRep);
//   distanceRep->InstantiateHandleRepresentation();
//   distanceRep->GetPoint1Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
//   distanceRep->GetPoint2Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
// 
//   // Add the distance to the list of widgets whose visibility is managed based
//   // on the reslice plane by the ResliceImageViewerMeasurements class
//   this->riw[i]->GetMeasurements()->AddItem(this->DistanceWidget[i]);
// 
//   this->DistanceWidget[i]->CreateDefaultRepresentation();
//   this->DistanceWidget[i]->EnabledOn();
// }
