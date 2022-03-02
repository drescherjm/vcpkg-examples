#########################################################################################
#
#	This module contains a few Helper Macros that add case insensitive list operations
#	among other functionality.
#
#########################################################################################

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    if (${value} STREQUAL ${value2})
      SET(${var} TRUE)
	  break()
    endif (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

#########################################################################################

MACRO(LIST_CONTAINS_IGNORE_CASE var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    if (${value} STREQUAL ${value2})
      SET(${var} TRUE)
	ELSE(${value} STREQUAL ${value2})
		STRING( TOUPPER ${value} UpcaseValue )
		STRING( TOUPPER ${value2} UpcaseValue2 )
		 if (${UpcaseValue} STREQUAL ${UpcaseValue2})
			set(${var} TRUE)
			break()
		 endif (${UpcaseValue} STREQUAL ${UpcaseValue2})
    endif (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS_IGNORE_CASE)

#########################################################################################
# Returns ${value}-NOTFOUND if not found or the matched string in the same case as the 
# list

MACRO(LIST_FIND_IGNORE_CASE var value)
  SET(${var} "${value}-NOTFOUND")
  FOREACH (value2 ${ARGN})
    if (${value} STREQUAL ${value2})
      SET(${var} ${value2})
	  break()
	ELSE(${value} STREQUAL ${value2})
		STRING( TOUPPER ${value} UpcaseValue )
		STRING( TOUPPER ${value2} UpcaseValue2 )
		 if (${UpcaseValue} STREQUAL ${UpcaseValue2})
			set(${var} ${value2})
			break()
		 endif (${UpcaseValue} STREQUAL ${UpcaseValue2})
    endif (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_FIND_IGNORE_CASE)

#########################################################################################

MACRO(LIST_FIND var value)
  SET(${var} "${value}-NOTFOUND")
  FOREACH (value2 ${ARGN})
    if (${value} STREQUAL ${value2})
      SET(${var} ${value2})
	  break()
    endif (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_FIND)

#########################################################################################

macro(LIST_TO_CSV_STRING my_string)
	unset( ___TEMP___ )
	FOREACH (value2 ${ARGN})
		if (___TEMP___)
			set ( ___TEMP___ ${___TEMP___} "," ${value2} )
		else(___TEMP___)
			set ( ___TEMP___ ${value2} )
		endif(___TEMP___)
	ENDFOREACH (value2) 
	set( ${my_string} "${___TEMP___}" )
	unset( ___TEMP___ )
endmacro(LIST_TO_CSV_STRING)

#########################################################################################

macro( print_list my_message)
	unset(__LIST_OUTPUT__)
	LIST_TO_CSV_STRING( __LIST_OUTPUT__ ${ARGN} )
	MESSAGE( STATUS "${my_message} " ${__LIST_OUTPUT__})
	unset(__LIST_OUTPUT__)
endmacro( print_list )

#########################################################################################

macro( conditional_define VariableName DefineName)
	if (${VariableName})
	
		message( STATUS "add_definitions(-D${DefineName})" )
		add_definitions(-D${DefineName})
		
	endif(${VariableName})
endmacro( conditional_define )

#########################################################################################

macro( conditional_not_define VariableName DefineName)
	if (NOT ${VariableName})
	
		message( STATUS "add_definitions(-D${DefineName})" )
		add_definitions(-D${DefineName})
		
	endif(NOT ${VariableName})
endmacro( conditional_not_define )

#########################################################################################

function( define_from_environment VariableName PackageName)
	if (NOT DEFINED ${VariableName})
		message( STATUS "${VariableName}=$ENV{${VariableName}}" )
		if (NOT "$ENV{${VariableName}}" STREQUAL "")
			set(${VariableName} $ENV{${VariableName}} CACHE PATH "Set the path for ${PackageName}" FORCE)
		endif (NOT "$ENV{${VariableName}}" STREQUAL "")
	endif(NOT DEFINED ${VariableName})
endfunction( define_from_environment)

#########################################################################################

function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

function(get_all_buildsystem_targets var)
    set(targets)
    get_all_buildsystem_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

function(get_all_imported_targets var)
    set(targets)
    get_all_imported_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

#########################################################################################

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
	
	get_property(current_targets DIRECTORY ${dir} PROPERTY IMPORTED_TARGETS)
    list(APPEND ${targets} ${current_targets})
	
endmacro()

#########################################################################################

macro(get_all_buildsystem_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_buildsystem_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
	
endmacro()

#########################################################################################

macro(get_all_imported_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_imported_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY IMPORTED_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

#########################################################################################

macro( myproject ProjectName )
	set( LOCAL_PROJECT_NAME ${ProjectName})
	set( LOCAL_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	set( LOCAL_PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

	set (${LOCAL_PROJECT_NAME}_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
	set (${LOCAL_PROJECT_NAME}_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
	set (${LOCAL_PROJECT_NAME}_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH_CLEAN})
	set (${LOCAL_PROJECT_NAME}_VERSION_STRING "${${LOCAL_PROJECT_NAME}_VERSION_MAJOR}.${${LOCAL_PROJECT_NAME}_VERSION_MINOR}.${${LOCAL_PROJECT_NAME}_VERSION_PATCH}")

	#RESET_OPTIMIZATIONS()
	initialize_project(${LOCAL_PROJECT_NAME})
	
endmacro( myproject )

#########################################################################################

macro( mysubproject ProjectName )
	set( LOCAL_SUBPROJECT_NAME ${ProjectName})
	set( LOCAL_SUBPROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	set( LOCAL_SUBPROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})			
endmacro( mysubproject )

#########################################################################################

macro(create_string_from_list my_str)
 foreach(VALUE ${ARGN})
  if ("${VALUE}" STREQUAL "${ARGV1}")
   set(result "${VALUE}")
  else()
   set(result "${result} ${VALUE}")
  endif()
 endforeach(VALUE)
 set(${my_str} ${result})
endmacro(create_string_from_list) 

#########################################################################################

macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()

#########################################################################################

macro ( setup_library_source_groups )
	
	set (extra_macro_args ${ARGN})
	list(LENGTH extra_macro_args num_extra_args)
	if (${num_extra_args} GREATER 0)
		
		SOURCE_GROUP("${ARGV0}\\Generated" FILES
			  ${${LOCAL_PROJECT_NAME}_RC_SRCS}
			  ${${LOCAL_PROJECT_NAME}_MOC_SRCS}
			  ${${LOCAL_PROJECT_NAME}_UI_HDRS}
		)

		SOURCE_GROUP("${ARGV0}\\Resources" FILES
			  ${${LOCAL_PROJECT_NAME}_UIS}
			  ${${LOCAL_PROJECT_NAME}_RCS}
		)
		SOURCE_GROUP("${ARGV0}\\Source Files" FILES
			 ${${LOCAL_PROJECT_NAME}_SRCS}
		)
			
		SOURCE_GROUP("${ARGV0}\\Header Files" FILES
			 ${${LOCAL_PROJECT_NAME}_EXT_HDRS}
			 ${${LOCAL_PROJECT_NAME}_MOC_HDRS}
		)
	else()
		SOURCE_GROUP("Generated" FILES
			  ${${LOCAL_PROJECT_NAME}_RC_SRCS}
			  ${${LOCAL_PROJECT_NAME}_MOC_SRCS}
			  ${${LOCAL_PROJECT_NAME}_UI_HDRS}
		)

		SOURCE_GROUP("Resources" FILES
			  ${${LOCAL_PROJECT_NAME}_UIS}
			  ${${LOCAL_PROJECT_NAME}_RCS}
		)
	endif()
	
endmacro ( setup_library_source_groups )

#########################################################################################

macro( add_library_module ModuleListName ModuleName )
	get_property(is_defined GLOBAL PROPERTY ${ModuleListName} DEFINED)
	if(NOT is_defined)
		define_property(GLOBAL PROPERTY ${ModuleListName} 
			BRIEF_DOCS "List of library modules"
			FULL_DOCS "List of library modules")
	endif()
	set_property(GLOBAL APPEND PROPERTY ${ModuleListName} "${ModuleName}")	
endmacro( add_library_module )

#########################################################################################

macro(init_lib_shared_static_option shared)
option (BUILD_${LOCAL_PROJECT_NAME}_SHARED		"Build ${LOCAL_PROJECT_NAME} as a shared library" ${shared})

if (BUILD_${LOCAL_PROJECT_NAME}_SHARED)
	set(${LOCAL_PROJECT_NAME}_SHARED SHARED)
else (BUILD_${LOCAL_PROJECT_NAME}_SHARED)
	set(${LOCAL_PROJECT_NAME}_SHARED)
endif (BUILD_${LOCAL_PROJECT_NAME}_SHARED)

endmacro(init_lib_shared_static_option)

#########################################################################################

# VERSION_STR_TO_INTS Macro
# This macro converts a version string into its three integer components.
#
# Usage:
#     VERSION_STR_TO_INTS( major minor patch version )
#
# Parameters:
#     major      The variable to store the major integer component in.
#     minor      The variable to store the minor integer component in.
#     patch      The variable to store the patch integer component in.
#     version    The version string to convert ("#.#.#" format).
macro( VERSION_STR_TO_INTS major minor patch version )

    string( REGEX REPLACE "([0-9]+).[0-9]+.[0-9]+" "\\1" ${major} ${version} )
    string( REGEX REPLACE "[0-9]+.([0-9]+).[0-9]+" "\\1" ${minor} ${version} )
    string( REGEX REPLACE "[0-9]+.[0-9]+.([0-9]+)" "\\1" ${patch} ${version} )

endmacro( VERSION_STR_TO_INTS )

#########################################################################################

macro( dump_all_variables )

	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()

endmacro( dump_all_variables )

#########################################################################################

macro( dump_all_variables_starting_with Name )

	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
		if ( _variableName MATCHES ^${Name}* )
			message(STATUS "${_variableName}=${${_variableName}}")
		endif()
	endforeach()

endmacro( dump_all_variables_starting_with )

#########################################################################################
