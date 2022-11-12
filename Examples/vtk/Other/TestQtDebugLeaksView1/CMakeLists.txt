myproject(TestQtDebugLeaksView1)

find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkGUISupportQt
)

if (NOT VTK_FOUND)
  message("Skipping ${LOCAL_PROJECT_NAME}: ${VTK_NOT_FOUND_MESSAGE}")
  return()
endif()

set( ${LOCAL_PROJECT_NAME}_SRCS
	TestQtDebugLeaksView.cxx
)

message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE ${${LOCAL_PROJECT_NAME}_SRCS} )
  target_link_libraries(${LOCAL_PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
else()
  # Prevent a "command line is too long" failure in Windows.
  set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
  add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE ${${LOCAL_PROJECT_NAME}_SRCS} )
  target_link_libraries(${LOCAL_PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${LOCAL_PROJECT_NAME}
    MODULES ${VTK_LIBRARIES}
    )
endif()

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER vtk/Other)