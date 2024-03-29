cmake_minimum_required(VERSION 3.17)

###############################################################################
# Setup VCPKG
###############################################################################

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(vcpkg-examples CXX)

###############################################################################

include(${PROJECT_SOURCE_DIR}/CMake/Scripts/GetCompilerVersion.cmake)
include(${PROJECT_SOURCE_DIR}/CMake/Scripts/NamingConvention.cmake)
include(${PROJECT_SOURCE_DIR}/CMake/Scripts/SetupProject.cmake)
include(${PROJECT_SOURCE_DIR}/CMake/Scripts/HelperMacros.cmake)
include(${PROJECT_SOURCE_DIR}/CMake/Scripts/SupportQt.cmake)


# if(DEFINED ENV{CMAKE_TOOLCHAIN_FILE} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  # set(CMAKE_TOOLCHAIN_FILE "$ENV{CMAKE_TOOLCHAIN_FILE}" CACHE STRING "")
# endif()

# if(CMAKE_TOOLCHAIN_FILE)
    # include(${CMAKE_TOOLCHAIN_FILE})
	
	# dump_all_variables()
	
# else()
	# message(FATAL_ERROR "The toolchain file must be specified")
# endif()

###############################################################################

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")

add_subdirectory(Examples)

get_all_targets(all_targets)
message(STATUS "All targets: ${all_targets}")

get_all_imported_targets(all_targets)
message(STATUS "All targets: ${all_targets}")