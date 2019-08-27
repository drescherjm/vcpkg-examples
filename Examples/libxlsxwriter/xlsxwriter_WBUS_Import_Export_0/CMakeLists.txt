myproject(xlswriter_wbus_export_0)

find_package(libxlsxwriter REQUIRED)

#include(${CMAKE_SOURCE_DIR}/Boost/CMake/SetupBoost.cmake)



# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${LOCAL_PROJECT_NAME} 
	src/main.cxx
	src/exportCSV.cxx
	src/stringHelpers.cxx
	src/CSVInfo.cxx
	Include/exportCSV.h
	Include/stringHelpers.h
	Include/CSVInfo.h
	Include/ColumnFormat.h
)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES
   CXX_STANDARD 17
)

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE
	 ${CMAKE_CURRENT_SOURCE_DIR}/Include	
)

target_link_libraries(${LOCAL_PROJECT_NAME} xlsxwriter)
