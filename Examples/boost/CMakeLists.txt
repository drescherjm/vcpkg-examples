
option( BUILD_BOOST_EXAMPLES 		"Build the boost examples." ON)

if(BUILD_BOOST_EXAMPLES)
	SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

	FOREACH(subdir ${SUBDIRS})
		ADD_SUBDIRECTORY(${subdir})
	ENDFOREACH()
endif()