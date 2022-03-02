
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

myproject(ImageTracerWidgetInsideContour)

find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkFiltersSources
  vtkIOImage
  vtkImagingSources
  vtkImagingStatistics
  vtkImagingStencil
  vtkInteractionStyle
  vtkInteractionWidgets
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2
  QUIET
)

if (NOT VTK_FOUND)
  message("Skipping ${LOCAL_PROJECT_NAME} : ${VTK_NOT_FOUND_MESSAGE}")
  return()
endif()

message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(${LOCAL_PROJECT_NAME}  MACOSX_BUNDLE ImageTracerWidgetInsideContour.cxx )
  target_link_libraries(${LOCAL_PROJECT_NAME}  PRIVATE ${VTK_LIBRARIES})
else()
  # Prevent a "command line is too long" failure in Windows.
  set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
  add_executable(${LOCAL_PROJECT_NAME}  MACOSX_BUNDLE ImageTracerWidgetInsideContour.cxx )
  target_link_libraries(${LOCAL_PROJECT_NAME}  PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${LOCAL_PROJECT_NAME} 
    MODULES ${VTK_LIBRARIES}
    )
endif()

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER vtk/Widgets)