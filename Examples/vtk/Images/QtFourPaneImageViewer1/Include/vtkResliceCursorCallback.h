#pragma once

#ifndef VTKRESLICECURSORCALLBACK_H
#define VTKRESLICECURSORCALLBACK_H

/////////////////////////////////////////////////////////////////////////////////////////

#include <vtkCommand.h>

class vtkObject;
class vtkImagePlaneWidget;
class vtkResliceCursorWidget;

/////////////////////////////////////////////////////////////////////////////////////////

class vtkResliceCursorCallback : public vtkCommand
{
public:
  static vtkResliceCursorCallback *New()
  { return new vtkResliceCursorCallback; }

  void Execute( vtkObject *caller, unsigned long ev,
                void *callData ) override;

  vtkResliceCursorCallback() {}
  vtkImagePlaneWidget* IPW[3];
  vtkResliceCursorWidget *RCW[3];
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // VTKRESLICECURSORCALLBACK_H
