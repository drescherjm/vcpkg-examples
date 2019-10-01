myproject(QtVTKRenderWindows_3)

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
if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif ()

include(${VTK_USE_FILE})

find_package(Qt5 COMPONENTS Widgets)
if (NOT TARGET Qt5::Widgets)
  message("Skipping example: ${Qt5_NOT_FOUND_MESSAGE}")
  return ()
endif ()

# Set your files and resources here
set(Srcs
  src/QtVTKRenderWindowsApp.cxx
  src/QtVTKRenderWindows.cxx
  src/vtkResliceCursorCallback.cxx
  src/vtkImageAlgorithmFilter.cxx)

set(Hdrs
  Include/QtVTKRenderWindows.h
  Include/vtkResliceCursorCallback.h
  Include/vtkImageAlgorithmFilter.h
  )

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
    Qt5::Widgets)
	

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE Include)
set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER vtk/Images)