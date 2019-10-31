myproject(ProgramOptions0)

find_package(Boost REQUIRED COMPONENTS program_options)

set(CMAKE_CXX_STANDARD 17)

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE src/ProgramOptions0.cxx)

target_include_directories( ${LOCAL_PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(${LOCAL_PROJECT_NAME} ${Boost_LIBRARIES})

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER boost)
