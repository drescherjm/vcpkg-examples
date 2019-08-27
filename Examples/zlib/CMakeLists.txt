myproject(zlib_example_0)

find_package(ZLIB REQUIRED)
add_executable(vcpkg_zlib_example_0 src/vcpkg_zlib_example_0.cxx)
target_link_libraries(vcpkg_zlib_example_0 ZLIB::ZLIB)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER zlib)