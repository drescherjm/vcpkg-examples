#include "ui_QtVTKRenderWindow.h"
#include "QtVTKRenderWindow.h"
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
#include "dicom/vtkDICOMReader.h"


QtVTKRenderWindow::QtVTKRenderWindow(int vtkNotUsed(argc), char* argv[])
{
	this->ui = new Ui_QtVTKRenderWindow;
	this->ui->setupUi(this);

	vtkSmartPointer< vtkDICOMReader > reader =
		vtkSmartPointer< vtkDICOMReader >::New();
	reader->SetFileName(argv[1]);
	reader->Update();

	vtkStringArray* stackNames = reader->GetStackIDs();


	int imageDims[3];
	reader->GetOutput()->GetDimensions(imageDims);

	riw = vtkSmartPointer< vtkResliceImageViewer >::New();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	riw->SetRenderWindow(renderWindow);

	this->ui->view->SetRenderWindow(riw->GetRenderWindow());
	riw->SetupInteractor(this->ui->view->GetRenderWindow()->GetInteractor());


	// make them all share the same reslice cursor object.
	vtkResliceCursorLineRepresentation* rep =
		vtkResliceCursorLineRepresentation::SafeDownCast(
			riw->GetResliceCursorWidget()->GetRepresentation());
	riw->SetResliceCursor(riw->GetResliceCursor());

	rep->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(vtkResliceImageViewer::SLICE_ORIENTATION_XY);

	riw->SetInputData(reader->GetOutput());
	riw->SetSliceOrientation(vtkResliceImageViewer::SLICE_ORIENTATION_XY); // enum { SLICE_ORIENTATION_YZ = 0, SLICE_ORIENTATION_XZ = 1, SLICE_ORIENTATION_XY = 2 }
	riw->SetResliceModeToAxisAligned();
	riw->SetColorWindow(512);
	riw->SetColorLevel(512);


	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	picker->SetTolerance(0.005);

	vtkSmartPointer<vtkProperty> ipwProp = vtkSmartPointer<vtkProperty>::New();

	vtkSmartPointer< vtkRenderer > ren = vtkSmartPointer< vtkRenderer >::New();

	this->ui->view->show();

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
};

void QtVTKRenderWindow::slotExit()
{
	qApp->exit();
}

void QtVTKRenderWindow::resliceMode(int mode)
{
	this->ui->thickModeCheckBox->setEnabled(mode ? 1 : 0);
	this->ui->blendModeGroupBox->setEnabled(mode ? 1 : 0);

	
		riw->SetResliceMode(mode ? 1 : 0);
		riw->GetRenderer()->ResetCamera();
		riw->Render();
	
}

void QtVTKRenderWindow::thickMode(int mode)
{
	riw->SetThickMode(mode ? 1 : 0);
	riw->Render();
}

void QtVTKRenderWindow::SetBlendMode(int m)
{
	
	vtkImageSlabReslice* thickSlabReslice = vtkImageSlabReslice::SafeDownCast(
		vtkResliceCursorThickLineRepresentation::SafeDownCast(
			riw->GetResliceCursorWidget()->GetRepresentation())->GetReslice());
	thickSlabReslice->SetBlendMode(m);
	riw->Render();
	
}

void QtVTKRenderWindow::SetBlendModeToMaxIP()
{
	this->SetBlendMode(VTK_IMAGE_SLAB_MAX);
}

void QtVTKRenderWindow::SetBlendModeToMinIP()
{
	this->SetBlendMode(VTK_IMAGE_SLAB_MIN);
}

void QtVTKRenderWindow::SetBlendModeToMeanIP()
{
	this->SetBlendMode(VTK_IMAGE_SLAB_MEAN);
}

void QtVTKRenderWindow::ResetViews()
{
	// Reset the reslice image views
	riw->Reset();

	// Render in response to changes.
	this->Render();
}

void QtVTKRenderWindow::Render()
{
	riw->Render();
	this->ui->view->GetRenderWindow()->Render();
}