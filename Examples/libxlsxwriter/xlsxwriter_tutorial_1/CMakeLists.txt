myproject(xlswriter_tutorial_1)

find_package(libxlsxwriter REQUIRED)

add_executable(${LOCAL_PROJECT_NAME} src/main.cxx)
target_link_libraries(${LOCAL_PROJECT_NAME} xlsxwriter)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER xlsxwriter)