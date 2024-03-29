/*=========================================================================

  Program:   Visualization Toolkit
  Module:    myVtkQtDebugLeaksModel.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "myVtkQtDebugLeaksModel.h"
#include "vtkDebugLeaks.h"

#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QPointer>
#include <QTextStream>
#include <QTimer>

//------------------------------------------------------------------------------
Q_DECLARE_METATYPE(vtkObjectBase*);

//------------------------------------------------------------------------------

class myVtkQtDebugLeaksModel::qObserver : public vtkDebugLeaksObserver
{
public:
  qObserver(myVtkQtDebugLeaksModel& model)
    : Model(model)
  {
    vtkDebugLeaks::SetDebugLeaksObserver(this);
  }

  ~qObserver() override { vtkDebugLeaks::SetDebugLeaksObserver(nullptr); }

  void ConstructingObject(vtkObjectBase* object) override { this->Model.addObject(object); }

  void DestructingObject(vtkObjectBase* object) override { this->Model.removeObject(object); }

  myVtkQtDebugLeaksModel& Model;

private:
  qObserver(const qObserver&) = delete;
  void operator=(const qObserver&) = delete;
};

//------------------------------------------------------------------------------
class VTKClassInfo
{
public:
  VTKClassInfo(const QString& className)
    : Count(0)
    , Name(className)
  {
  }
  int Count;
  QString Name;
  QList<vtkObjectBase*> Objects;
};

//------------------------------------------------------------------------------
class myVtkQtDebugLeaksModel::qInternal
{
public:
  qInternal()
    : ProcessPending(false)
  {
  }

  bool ProcessPending;
  QList<QString> Classes;
  QList<VTKClassInfo> ClassInfo;
  QList<vtkObjectBase*> ObjectsToProcess;
  QHash<vtkObjectBase*, VTKClassInfo*> ObjectMap;
  QHash<QString, QPointer<ReferenceCountModel>> ReferenceModels;
};

//------------------------------------------------------------------------------
myVtkQtDebugLeaksModel::myVtkQtDebugLeaksModel(QObject* p)
  : QStandardItemModel(0, 2, p)
{
  this->Internal = new qInternal;
  this->Observer = new qObserver(*this);

  this->setHeaderData(0, Qt::Horizontal, QObject::tr("Class Name"));
  this->setHeaderData(1, Qt::Horizontal, QObject::tr("Class Count"));

  this->connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), SLOT(onAboutToQuit()));
}

//------------------------------------------------------------------------------
myVtkQtDebugLeaksModel::~myVtkQtDebugLeaksModel()
{
  delete this->Observer;
  delete this->Internal;
}

//------------------------------------------------------------------------------
void myVtkQtDebugLeaksModel::onAboutToQuit()
{
  delete this->Observer;
  this->Observer = nullptr;
}

//------------------------------------------------------------------------------
void myVtkQtDebugLeaksModel::addObject(vtkObjectBase* object)
{
  this->Internal->ObjectsToProcess.append(object);
  if (!this->Internal->ProcessPending)
  {
    this->Internal->ProcessPending = true;
    QTimer::singleShot(0, this, SLOT(processPendingObjects()));
  }
}

//------------------------------------------------------------------------------
void myVtkQtDebugLeaksModel::processPendingObjects()
{
  this->Internal->ProcessPending = false;
  Q_FOREACH (vtkObjectBase* object, this->Internal->ObjectsToProcess)
  {
    this->registerObject(object);
  }
  this->Internal->ObjectsToProcess.clear();
}

//------------------------------------------------------------------------------
void myVtkQtDebugLeaksModel::registerObject(vtkObjectBase* object)
{
  QString className = object->GetClassName();
  int indexOf = this->Internal->Classes.indexOf(className);
  if (indexOf < 0)
  {
    this->Internal->Classes.append(className);
    this->Internal->ClassInfo.append(VTKClassInfo(className));

    indexOf = this->rowCount();
    this->insertRow(indexOf);
    this->setData(this->index(indexOf, 0), className);
    this->setData(this->index(indexOf, 1), 0);
  }

  VTKClassInfo& classInfo = this->Internal->ClassInfo[indexOf];
  classInfo.Count += 1;
  classInfo.Objects.append(object);
  this->Internal->ObjectMap[object] = &classInfo;
  this->setData(this->index(indexOf, 1), classInfo.Count);

  ReferenceCountModel* model = this->Internal->ReferenceModels.value(className, nullptr);
  if (model)
  {
    model->addObject(object);
  }
}

//------------------------------------------------------------------------------
void myVtkQtDebugLeaksModel::removeObject(vtkObjectBase* object)
{
  VTKClassInfo* classInfo = this->Internal->ObjectMap.value(object, 0);
  if (classInfo)
  {
    QString className = classInfo->Name;
    int row = this->Internal->Classes.indexOf(className);
    classInfo->Count -= 1;
    classInfo->Objects.removeOne(object);
    this->Internal->ObjectMap.remove(object);

    if (classInfo->Count <= 0)
    {
      this->Internal->Classes.removeAt(row);
      this->Internal->ClassInfo.removeAt(row);
      this->removeRow(row);
    }
    else
    {
      this->setData(this->index(row, 1), classInfo->Count);
    }

    ReferenceCountModel* model = this->Internal->ReferenceModels.value(className, nullptr);
    if (model)
    {
      model->removeObject(object);
    }
  }
  else
  {
    this->Internal->ObjectsToProcess.removeOne(object);
  }
}

//------------------------------------------------------------------------------
QList<vtkObjectBase*> myVtkQtDebugLeaksModel::getObjects(const QString& className)
{
  int indexOf = this->Internal->Classes.indexOf(className);
  if (indexOf < 0)
  {
    qWarning() << "myVtkQtDebugLeaksModel::getObjects: bad class name:" << className;
    return QList<vtkObjectBase*>();
  }

  VTKClassInfo& classInfo = this->Internal->ClassInfo[indexOf];
  return classInfo.Objects;
}

//------------------------------------------------------------------------------
QStandardItemModel* myVtkQtDebugLeaksModel::referenceCountModel(const QString& className)
{
  ReferenceCountModel* model = this->Internal->ReferenceModels.value(className, nullptr);
  if (!model)
  {
    model = new ReferenceCountModel(this);
    this->Internal->ReferenceModels[className] = model;
    Q_FOREACH (vtkObjectBase* obj, this->getObjects(className))
    {
      model->addObject(obj);
    }
  }

  return model;
}

//------------------------------------------------------------------------------
Qt::ItemFlags myVtkQtDebugLeaksModel::flags(const QModelIndex& modelIndex) const
{
  Q_UNUSED(modelIndex);
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

//------------------------------------------------------------------------------
ReferenceCountModel::ReferenceCountModel(QObject* p)
  : QStandardItemModel(0, 2, p)
{
  this->setHeaderData(0, Qt::Horizontal, QObject::tr("Pointer"));
  this->setHeaderData(1, Qt::Horizontal, QObject::tr("Reference Count"));
  QTimer::singleShot(100, this, SLOT(updateReferenceCounts()));
}

//------------------------------------------------------------------------------
ReferenceCountModel::~ReferenceCountModel() = default;

//------------------------------------------------------------------------------
QString ReferenceCountModel::pointerAsString(void* ptr)
{
  QString ptrStr;
  QTextStream stream(&ptrStr);
  stream << ptr;
  return ptrStr;
}

//------------------------------------------------------------------------------
void ReferenceCountModel::addObject(vtkObjectBase* obj)
{
  int row = this->rowCount();
  this->insertRow(row);
  this->setData(this->index(row, 0), this->pointerAsString(obj));
  this->setData(this->index(row, 0), QVariant::fromValue(obj), Qt::UserRole);
  this->setData(this->index(row, 1), obj->GetReferenceCount());
}

//------------------------------------------------------------------------------
void ReferenceCountModel::removeObject(vtkObjectBase* obj)
{
  QString pointerString = this->pointerAsString(obj);

  for (int i = 0; i < this->rowCount(); ++i)
  {
    if (this->data(this->index(i, 0)) == pointerString)
    {
      this->removeRow(i);
      return;
    }
  }
}

//------------------------------------------------------------------------------
void ReferenceCountModel::updateReferenceCounts()
{
  for (int row = 0; row < this->rowCount(); ++row)
  {
    QVariant pointerVariant = this->data(this->index(row, 0), Qt::UserRole);
    vtkObjectBase* obj = pointerVariant.value<vtkObjectBase*>();
    this->setData(this->index(row, 1), obj->GetReferenceCount());
  }

  QTimer::singleShot(100, this, SLOT(updateReferenceCounts()));
}

//------------------------------------------------------------------------------
Qt::ItemFlags ReferenceCountModel::flags(const QModelIndex& modelIndex) const
{
  Q_UNUSED(modelIndex);
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

