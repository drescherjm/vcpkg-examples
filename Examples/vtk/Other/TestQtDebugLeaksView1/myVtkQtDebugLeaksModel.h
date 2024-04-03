/*=========================================================================

  Program:   Visualization Toolkit
  Module:    myVtkQtDebugLeaksModel.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   myVtkQtDebugLeaksModel
 * @brief   model class that observes the vtkDebugLeaks singleton
 *
 *
 * This class is used internally by the vtkQtDebugLeaksView.  It installs an
 * observer on the vtkDebugLeaks singleton and uses the observer to maintain
 * a model of all vtkObjectBase derived objects that are alive in memory.
 */

#ifndef myVtkQtDebugLeaksModel_h
#define myVtkQtDebugLeaksModel_h

#include <QStandardItemModel>

class vtkObjectBase;

class myVtkQtDebugLeaksModel : public QStandardItemModel
{
  Q_OBJECT

public:
  myVtkQtDebugLeaksModel(QObject* p = nullptr);
  ~myVtkQtDebugLeaksModel() override;

  /**
   * Get the list of objects in the model that have the given class name
   */
  QList<vtkObjectBase*> getObjects(const QString& className);

  /**
   * Return an item model that contains only objects with the given class name.
   * The model has two columns: object address (string), object reference count (integer)
   * The caller is allowed to reparent or delete the returned model.
   */
  QStandardItemModel* referenceCountModel(const QString& className);

protected Q_SLOTS:

  void addObject(vtkObjectBase* object);
  void removeObject(vtkObjectBase* object);
  void registerObject(vtkObjectBase* object);
  void processPendingObjects();
  void onAboutToQuit();

  // Inherited method from QAbstractItemModel
  Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
  class qInternal;
  qInternal* Internal;

  class qObserver;
  qObserver* Observer;

  Q_DISABLE_COPY(myVtkQtDebugLeaksModel);
};

// TODO - move to private
//-----------------------------------------------------------------------------
class ReferenceCountModel : public QStandardItemModel
{
  Q_OBJECT

public:
  ReferenceCountModel(QObject* p = nullptr);
  ~ReferenceCountModel() override;
  void addObject(vtkObjectBase* obj);
  void removeObject(vtkObjectBase* obj);
  QString pointerAsString(void* ptr);

  // Inherited method from QAbstractItemModel
  Qt::ItemFlags flags(const QModelIndex& index) const override;

protected Q_SLOTS:
  void updateReferenceCounts();
};

#endif
// VTK-HeaderTest-Exclude: myVtkQtDebugLeaksModel.h
