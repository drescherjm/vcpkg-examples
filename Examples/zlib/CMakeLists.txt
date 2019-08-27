myproject(zlib_example_0)

find_package(ZLIB REQUIRED)
add_executable(${LOCAL_PROJECT_NAME} src/vcpkg_zlib_example_0.cxx)
target_link_libraries(${LOCAL_PROJECT_NAME} ZLIB::ZLIB)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER zlib)