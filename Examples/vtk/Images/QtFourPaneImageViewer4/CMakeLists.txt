myproject(QtVTKRenderWindows_4)

if (VTK_VERSION VERSION_LESS "8.90.0")
find_package(VTK
  COMPONENTS
    vtkCommonCore
    vtkGUISupportQt
    vtkIOImage
    vtkImagingColor
    vtkImagingGeneral
    vtkInteractionImage
    vtkInteractionStyle
    vtkInteractionWidgets)
	
	include(${VTK_USE_FILE})
	
else()
find_package(VTK
  COMPONENTS
    CommonCore
    GUISupportQt
    IOImage
    ImagingColor
    ImagingGeneral
    InteractionImage
    InteractionStyle
    InteractionWidgets)
endif()
	
if (NOT VTK_FOUND)
  message("Skipping example: ${LOCAL_PROJECT_NAME} because VTK is not found.")
  return ()
endif ()

if (NOT TARGET VTK::DICOM)
  message("Skipping example: ${LOCAL_PROJECT_NAME} because VTK::DICOM is not found.")
  return ()
endif ()

find_package(Qt5 COMPONENTS Widgets)
if (NOT TARGET Qt5::Widgets)
  message("Skipping example: ${LOCAL_PROJECT_NAME} because Qt::Widgets is not found.")
  return ()
endif ()


# Set your files and resources here
set(Srcs
  src/QtVTKRenderWindowsApp.cxx
  src/QtVTKRenderWindows.cxx
  src/vtkResliceCursorCallback.cxx)

set(Hdrs
  Include/QtVTKRenderWindows.h
  Include/vtkResliceCursorCallback.h)

set(UIs
  rc/QtVTKRenderWindows.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS rc)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# CMAKE_AUTOMOC in ON so the MocHdrs will be automatically wrapped.
add_executable(${LOCAL_PROJECT_NAME} 
  ${Srcs} ${Hdrs} ${UIs})
target_link_libraries(${LOCAL_PROJECT_NAME} 
  PRIVATE
    ${VTK_LIBRARIES}
	VTK::DICOM
    Qt5::Widgets)
	

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE Include)
set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER vtk/Images)