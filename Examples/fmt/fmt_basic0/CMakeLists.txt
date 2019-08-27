myproject(fmt_example_0)

find_package(fmt REQUIRED)

add_executable(${LOCAL_PROJECT_NAME} src/fmt_example_0.cxx)
target_link_libraries(${LOCAL_PROJECT_NAME} fmt::fmt)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER fmt)