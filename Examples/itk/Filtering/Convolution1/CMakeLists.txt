myproject(Convolution1)

find_package(ITK REQUIRED )
include(${ITK_USE_FILE})

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE src/Convolution1.cxx  Include/Convolution1.h)


#message( FATAL_ERROR ITK_LIBRARIES=${ITK_LIBRARIES})

target_link_directories(${LOCAL_PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:X:/vcpkg/installed/x64-windows/debug/lib> $<$<CONFIG:Release>:X:/vcpkg/installed/x64-windows/lib>)

target_link_libraries(${LOCAL_PROJECT_NAME} ITKCommon ITKIOImageBase ITKIOBMP ITKIOGDCM ITKIOPNG ITKIOJPEG ITKIOJPEG2000 ITKIOTIFF ITKIOMINC ITKIOMRC ITKIONIFTI ITKIONRRD ITKIOStimulate ITKIOMeta ITKIOBioRad ITKIOBruker ITKIOGIPL ITKIOHDF5 ITKIOGE ITKIOLSM ITKIOVTK ITKVTK)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER itk/Filtering)
