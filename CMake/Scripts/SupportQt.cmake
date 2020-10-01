#########################################################################################

if(WIN32)
	OPTION( SYSTEM_FORCE_CONSOLE_WINDOW "Make all Qt applications console applications for debugging purposes." OFF)
endif(WIN32)

#########################################################################################

if ( NOT DEFINED ${${PROJECT_NAME}_QT_VERSION} )
	if (NOT "$ENV{QT_VERSION}" STREQUAL "")
		set( ENV_QT_VER_MAJOR )
		set( ENV_QT_VER_MINOR )
		set( ENV_QT_VER_PATCH )
		
		VERSION_STR_TO_INTS( ENV_QT_VER_MAJOR ENV_QT_VER_MINOR ENV_QT_VER_PATCH $ENV{QT_VERSION} )
		
		if ( (${ENV_QT_VER_MAJOR} EQUAL "4") OR (${ENV_QT_VER_MAJOR} EQUAL "5") ) 
			message( STATUS "Using environent to set Qt version to ${ENV_QT_VER_MAJOR}" )
			set(${PROJECT_NAME}_QT_VERSION ${ENV_QT_VER_MAJOR} CACHE STRING "Expected Qt version")
		endif()
		
	endif()
	
	if ( NOT DEFINED ${${PROJECT_NAME}_QT_VERSION} )
		if (NOT MSVC14)
			# We could not get the version from the environment so assume the default is 4..
			set(${PROJECT_NAME}_QT_VERSION "4" CACHE STRING "Expected Qt version")
		else()
			set(${PROJECT_NAME}_QT_VERSION "5" CACHE STRING "Expected Qt version")
		endif()
	endif()
endif()

mark_as_advanced(${PROJECT_NAME}_QT_VERSION)

set_property(CACHE ${PROJECT_NAME}_QT_VERSION PROPERTY STRINGS 4 5)

if(NOT (${PROJECT_NAME}_QT_VERSION VERSION_EQUAL "4" OR ${PROJECT_NAME}_QT_VERSION VERSION_EQUAL "5"))
	message(FATAL_ERROR "Expected value for ${PROJECT_NAME}_QT_VERSION is either '4' or '5'")
endif()

#########################################################################################

if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
	define_from_environment(QT_DIR Qt)
	
	#find_path(QT_CMAKE_PATH NAME CMake PATHS ${QT_DIR} ${QT_DIR}/qtbase NO_DEFAULT_PATH)
	
	if( IS_DIRECTORY ${QT_DIR}/qtbase/lib/cmake ) 
		#set_property(CACHE QT_CMAKE_PATH PROPERTY PATH ${QT_DIR}/qtbase/lib/cmake)
		set(QT_CMAKE_PATH ${QT_DIR}/qtbase/lib/cmake CACHE PATH "Set the path to help cmake find Qt5 via the .cmake files in the Qt5 build." FORCE)
	
	elseif ( IS_DIRECTORY ${QT_DIR}/lib/cmake ) 
		set(QT_CMAKE_PATH ${QT_DIR}/lib/cmake CACHE PATH "Set the path to help cmake find Qt5 via the .cmake files in the Qt5 build." FORCE)
	endif()
	
endif()

#########################################################################################

macro( find_qt5_packages )
	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		foreach(MODULE ${QT_MODULES}) 
			if ( NOT TARGET ${MODULE} ) 
			
				STRING( REPLACE ":" "" COMPONENT ${MODULE})
				message( STATUS "Looking for module: ${MODULE} ${COMPONENT} in ${QT_CMAKE_PATH}")
				
				find_package(${COMPONENT} REQUIRED PATHS ${QT_CMAKE_PATH})
				include_directories(${${COMPONENT}_INCLUDE_DIRS})
				add_definitions(${${COMPONENT}_DEFINITIONS})
				
			endif()
		endforeach()
	else()
		
	endif(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		
endmacro( find_qt5_packages )

#########################################################################################

macro ( setup_library_qt_wrap_support )

	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		
		# this command will generate rules that will run rcc on all files from UPMC_LA_RCS
		# in result UPMC_LA_RC_SRCS variable will contain paths to files produced by rcc
		QT5_ADD_RESOURCES( ${LOCAL_PROJECT_NAME}_RC_SRCS ${${LOCAL_PROJECT_NAME}_RCS} )

		# and finally this will run moc:
		QT5_WRAP_CPP( ${LOCAL_PROJECT_NAME}_MOC_SRCS ${${LOCAL_PROJECT_NAME}_MOC_HDRS} )
		
		# this will run uic on .ui files:
		QT5_WRAP_UI( ${LOCAL_PROJECT_NAME}_UI_HDRS ${${LOCAL_PROJECT_NAME}_UIS} )
	else()
		# this command will generate rules that will run rcc on all files from UPMC_LA_RCS
		# in result UPMC_LA_RC_SRCS variable will contain paths to files produced by rcc
		QT4_ADD_RESOURCES( ${LOCAL_PROJECT_NAME}_RC_SRCS ${${LOCAL_PROJECT_NAME}_RCS} )

		# and finally this will run moc:
		if (USE_MSVC_PCH AND USE_MSVC_PCH_WITH_MOC)
			PCH_QT4_WRAP_CPP( ${LOCAL_PROJECT_NAME}PCH ${LOCAL_PROJECT_NAME}_MOC_SRCS ${${LOCAL_PROJECT_NAME}_MOC_HDRS} )
		else (USE_MSVC_PCH AND USE_MSVC_PCH_WITH_MOC) 
			 QT4_WRAP_CPP( ${LOCAL_PROJECT_NAME}_MOC_SRCS ${${LOCAL_PROJECT_NAME}_MOC_HDRS} )
		endif (USE_MSVC_PCH AND USE_MSVC_PCH_WITH_MOC)

		# this will run uic on .ui files:
		QT4_WRAP_UI( ${LOCAL_PROJECT_NAME}_UI_HDRS ${${LOCAL_PROJECT_NAME}_UIS} )

		#setup_library_source_groups()
	endif()

endmacro ( setup_library_qt_wrap_support )

#########################################################################################

function( setup_qt_executable ExecTarget)

	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		# this command will generate rules that will run rcc on all files from ${ExecTarget}_RCS
		# in result ${ExecTarget}_RC_SRCS variable will contain paths to files produced by rcc
		QT5_ADD_RESOURCES( ${ExecTarget}_RC_SRCS ${${ExecTarget}_RCS} )

		# and finally this will run moc:
		QT5_WRAP_CPP( ${ExecTarget}_MOC_SRCS ${${ExecTarget}_MOC_HDRS} )

		# this will run uic on .ui files:
		QT5_WRAP_UI( ${ExecTarget}_UI_HDRS ${${ExecTarget}_UIS} )
	else()
		# this command will generate rules that will run rcc on all files from ${ExecTarget}_RCS
		# in result ${ExecTarget}_RC_SRCS variable will contain paths to files produced by rcc
		QT4_ADD_RESOURCES( ${ExecTarget}_RC_SRCS ${${ExecTarget}_RCS} )

		# and finally this will run moc:
		QT4_WRAP_CPP( ${ExecTarget}_MOC_SRCS ${${ExecTarget}_MOC_HDRS} )

		# this will run uic on .ui files:
		QT4_WRAP_UI( ${ExecTarget}_UI_HDRS ${${ExecTarget}_UIS} )
	endif()

	SOURCE_GROUP("Generated" FILES
		  ${${ExecTarget}_RC_SRCS}
		  ${${ExecTarget}_MOC_SRCS}
		  ${${ExecTarget}_UI_HDRS}
	)

	SOURCE_GROUP("Resources" FILES
		  ${${ExecTarget}_UIS}
		  ${${ExecTarget}_RCS}
	)

	# Default GUI type is blank
	set(GUI_TYPE "")

	#-- Configure the OS X Bundle Plist
	if (APPLE)
	   SET(GUI_TYPE MACOSX_BUNDLE)
	elseif(WIN32)
		if ( SYSTEM_FORCE_CONSOLE_WINDOW )
			set(GUI_TYPE)
		else( SYSTEM_FORCE_CONSOLE_WINDOW )
			set(GUI_TYPE WIN32)
		endif(SYSTEM_FORCE_CONSOLE_WINDOW)
	endif()

	add_executable(${ExecTarget} ${GUI_TYPE} ${${ExecTarget}_SRCS} 
		${${ExecTarget}_MOC_SRCS} 
		${${ExecTarget}_HDRS}
		${${ExecTarget}_MOC_HDRS}
		${${ExecTarget}_UI_HDRS}
		${${ExecTarget}_RC_SRCS}
	)
	
endfunction( setup_qt_executable )

#########################################################################################

function( install_qt_designer_plugin PluginTargetName)

	#Install the designer plugin
	if(QT_PLUGINS_DIR)
	  # Legacy FindQt4.
	  install(TARGETS ${PluginTargetName}
		RUNTIME DESTINATION "${QT_PLUGINS_DIR}/designer"
	  )
	else()
	  # Call qmake to query the plugin installation directory.
	  get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
	  execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
		OUTPUT_VARIABLE QT_INSTALL_PLUGINS OUTPUT_STRIP_TRAILING_WHITESPACE
	  )
	  install(TARGETS ${PluginTargetName}
		RUNTIME DESTINATION "${QT_INSTALL_PLUGINS}/designer"
	  )
	endif()

endfunction(install_qt_designer_plugin)

#########################################################################################

function( setup_qt_plugin PluginTargetName)
	set_target_properties(${PluginTargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug/plugins)
	set_target_properties(${PluginTargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release/plugins)
	set_target_properties(${PluginTargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/plugins)
	set_target_properties(${PluginTargetName} PROPERTIES FOLDER Plugins)
	
	#get_
	
	get_target_property(RELEASE_POSTFIX ${PluginTargetName} RELEASE_POSTFIX)
	SET_PROPERTY(GLOBAL APPEND PROPERTY UPMC_PACKAGE_PLUGINS "${CMAKE_BINARY_DIR}/bin/Release/plugins/${PluginTargetName}${RELEASE_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}")
	
	#print_target_properties(${PluginTargetName})
	
	#echo_target_property(${PluginTargetName} SUFFIX)
	
	#echo_target(${PluginTargetName} )
	
	
	#get_target_property(RELEASE_POSTFIX ${PluginTargetName} RELEASE_POSTFIX)
	#get_target_property(RELEASE_POSTFIX ${PluginTargetName} SUFFIX)
	
	#message( FATAL_ERROR "Stop For Debugging! ${CMAKE_BINARY_DIR}/bin/Release/plugins/${PluginTargetName}${RELEASE_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}")
			
	if(PACKAGE_FOR_INSTALL)
		INSTALL(TARGETS ${PluginTargetName}
			DESTINATION bin/plugins
			COMPONENT Plugins
		)
	endif(PACKAGE_FOR_INSTALL)
	
endfunction( setup_qt_plugin)

#########################################################################################

macro( QT45_WRAP_CPP )

	#NOTE: Keep these as macros. functions will not work!

	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		#message( STATUS "QT5_WRAP_CPP( ${ARGV} )" )
		QT5_WRAP_CPP( ${ARGV} )
	else()
		#message( STATUS "QT4_WRAP_CPP( ${ARGV} )" )
		QT4_WRAP_CPP( ${ARGV} )
	endif()

endmacro ( QT45_WRAP_CPP )

#########################################################################################

macro( QT45_WRAP_UI )

	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		QT5_WRAP_UI( ${ARGV} )
	else()
		QT4_WRAP_UI( ${ARGV} )
	endif()

endmacro ( QT45_WRAP_UI )

#########################################################################################

macro( QT45_ADD_RESOURCES )

	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		QT5_ADD_RESOURCES( ${ARGV} )
	else()
		QT4_ADD_RESOURCES( ${ARGV} )
	endif()

endmacro ( QT45_ADD_RESOURCES )

#########################################################################################

macro( get_qt_base_dir )
	if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
		if ( NOT TARGET Qt5::Core ) 
			message( WARNING "Please call get_qt_base_dir after you find Qt" )
		else()
			get_target_property(_QtCore_location Qt5::Core LOCATION)
			get_filename_component(QT_BASE_DIR "${_QtCore_location}/../../" ABSOLUTE)
		endif()
	else()
		define_from_environment( QT_BASE_DIR Qt )
		#message( WARNING "Qt4 support of get_qt_base_dir is not implemented!" )
	endif()
endmacro( get_qt_base_dir )

#########################################################################################
