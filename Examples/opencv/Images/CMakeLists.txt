
option( BUILD_OPENCV_IMAGES_EXAMPLES 		"Build the opencv images examples." ON)

if(BUILD_OPENCV_IMAGES_EXAMPLES)
	SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

	FOREACH(subdir ${SUBDIRS})
		ADD_SUBDIRECTORY(${subdir})
	ENDFOREACH()
endif()