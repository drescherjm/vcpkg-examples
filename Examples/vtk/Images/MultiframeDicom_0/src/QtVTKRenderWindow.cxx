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
	

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	picker->SetTolerance(0.005);

	vtkSmartPointer<vtkProperty> ipwProp = vtkSmartPointer<vtkProperty>::New();

	vtkSmartPointer< vtkRenderer > ren = vtkSmartPointer< vtkRenderer >::New();

	/*
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	this->ui->view4->SetRenderWindow(renderWindow);
	this->ui->view4->GetRenderWindow()->AddRenderer(ren);
	vtkRenderWindowInteractor* iren = this->ui->view4->GetInteractor();

	for (int i = 0; i < 3; i++)
	{
		planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
		planeWidget[i]->SetInteractor(iren);
		planeWidget[i]->SetPicker(picker);
		planeWidget[i]->RestrictPlaneToVolumeOn();
		double color[3] = { 0, 0, 0 };
		color[i] = 1;
		planeWidget[i]->GetPlaneProperty()->SetColor(color);

		color[0] /= 4.0;
		color[1] /= 4.0;
		color[2] /= 4.0;
		// This sets the background color on the 3 slice views to match the color
		// set on the 4th 3D Image plane widget.
		riw[i]->GetRenderer()->SetBackground(color);

		planeWidget[i]->SetTexturePlaneProperty(ipwProp);
		planeWidget[i]->TextureInterpolateOff();
		planeWidget[i]->SetResliceInterpolateToLinear();
		planeWidget[i]->SetInputConnection(reader->GetOutputPort());
		planeWidget[i]->SetPlaneOrientation(i);
		planeWidget[i]->SetSliceIndex(imageDims[i] / 2);
		planeWidget[i]->DisplayTextOn();
		planeWidget[i]->SetDefaultRenderer(ren);
		planeWidget[i]->SetWindowLevel(1358, -27);
		planeWidget[i]->On();
		planeWidget[i]->InteractionOn();
	}

	vtkSmartPointer<vtkResliceCursorCallback> cbk = vtkSmartPointer<vtkResliceCursorCallback>::New();

	for (int i = 0; i < 3; i++)
	{
		cbk->IPW[i] = planeWidget[i];
		cbk->RCW[i] = riw[i]->GetResliceCursorWidget();
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::ResliceAxesChangedEvent, cbk);
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::WindowLevelEvent, cbk);
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::ResliceThicknessChangedEvent, cbk);
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::ResetCursorEvent, cbk);
		riw[i]->GetInteractorStyle()->AddObserver(
			vtkCommand::WindowLevelEvent, cbk);

		// Make them all share the same color map.
		riw[i]->SetLookupTable(riw[0]->GetLookupTable());
		planeWidget[i]->GetColorMap()->SetLookupTable(riw[0]->GetLookupTable());
		//planeWidget[i]->GetColorMap()->SetInput(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap()->GetInput());
		planeWidget[i]->SetColorMap(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());

	}
	*/

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

	// Also sync the Image plane widget on the 3D top right view with any
	// changes to the reslice cursor.

	/*
	vtkPlaneSource* ps = static_cast<vtkPlaneSource*>(
		planeWidget[i]->GetPolyDataAlgorithm());
	ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
	ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
	

	// If the reslice plane has modified, update it on the 3D widget
	this->planeWidget[i]->UpdatePlacement();
	*/

	// Render in response to changes.
	this->Render();
}

void QtVTKRenderWindow::Render()
{
	riw->Render();
	this->ui->view->GetRenderWindow()->Render();
}