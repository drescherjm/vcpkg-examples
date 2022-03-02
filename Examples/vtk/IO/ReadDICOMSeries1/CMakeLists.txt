if (USE_VTKDICOM)
	myproject(vtkReadDICOMSeries1)

	find_package(VTK COMPONENTS 
	  vtkCommonCore
	  vtkIOImage
	  vtkInteractionImage
	  vtkInteractionStyle
	  vtkRenderingCore
	  vtkRenderingFreeType
	  vtkRenderingOpenGL2 
	  vtk
	  QUIET)
	if (NOT VTK_FOUND)
	  message("Skipping ${LOCAL_PROJECT_NAME}: ${VTK_NOT_FOUND_MESSAGE}")
	  return ()
	endif()
	message (STATUS "VTK_VERSION: ${VTK_VERSION}")
	if (VTK_VERSION VERSION_LESS "8.90.0")
	  # old system
	  include(${VTK_USE_FILE})
	  add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE ReadDICOMSeries.cxx )
	  target_link_libraries(${LOCAL_PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
	else ()
	  # include all components
	  add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE ReadDICOMSeries.cxx )
	  target_link_libraries(${LOCAL_PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
	  # vtk_module_autoinit is needed
	  vtk_module_autoinit(
		TARGETS ${LOCAL_PROJECT_NAME}
		MODULES ${VTK_LIBRARIES}
		)
	endif () 

	set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER vtk/IO)
endif()