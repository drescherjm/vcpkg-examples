
option( BUILD_VTK_EXAMPLES 		"Build the vtk examples." ON)

if(BUILD_VTK_EXAMPLES)

	option (USE_VTK_DICOM_LIBRARY		"Use the vtk-dicom library instead of GDCM to read DICOM files." ON)

	SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

	FOREACH(subdir ${SUBDIRS})
		ADD_SUBDIRECTORY(${subdir})
	ENDFOREACH()
endif()