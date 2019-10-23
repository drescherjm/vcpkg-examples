myproject(Convolution0)

find_package(ITK REQUIRED )
include(${ITK_USE_FILE})

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE src/Convolution0.cxx  Include/Convolution0.h)

target_link_libraries(${LOCAL_PROJECT_NAME} ${ITK_LIBRARIES})

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER itk/Filtering)
