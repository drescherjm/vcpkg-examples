myproject(vcpkg_zlib_example_0)

#include CMAKE_TOOLCHAIN_FILE=X:\vcpkg\scripts\buildsystems\vcpkg.cmake

set(CMAKE_TOOLCHAIN_FILE "X:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Path for vcpkg.cmake")

if(CMAKE_TOOLCHAIN_FILE)
    include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

find_package(ZLIB REQUIRED)
add_executable(vcpkg_zlib_example_0 src/vcpkg_zlib_example_0.cxx)
target_link_libraries(vcpkg_zlib_example_0 ZLIB::ZLIB)