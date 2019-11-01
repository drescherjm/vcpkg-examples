myproject(CLI_0)

find_package(CLI11 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE src/cli_0.cxx)

target_link_libraries(${LOCAL_PROJECT_NAME} CLI11::CLI11)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER CLI11)
