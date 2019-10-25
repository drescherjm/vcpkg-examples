myproject(Convolution3)

find_package(ITK REQUIRED )
include(${ITK_USE_FILE})

set(CMAKE_CXX_STANDARD 17)

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE src/Convolution3.cxx  Include/Convolution3.h)

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE Include)

set(ITK_MODULES ITKCommon ITKIOImageBase ITKIOBMP ITKIOGDCM ITKIOPNG ITKIOJPEG ITKIOJPEG2000 ITKIOTIFF ITKIOMINC ITKIOMRC ITKIONIFTI ITKIONRRD ITKIOStimulate ITKIOMeta ITKIOBioRad ITKIOBruker ITKIOGIPL ITKIOHDF5 ITKIOGE ITKIOLSM ITKIOVTK ITKVTK)

if (ITKVtkGlue_LOADED) 
	set(ITK_MODULES ITKVtkGlue ${ITK_MODULES})
	
	target_compile_definitions( ${LOCAL_PROJECT_NAME} PRIVATE ENABLE_QUICKVIEW)
endif()

target_link_directories(${LOCAL_PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:X:/vcpkg/installed/x64-windows/debug/lib> $<$<CONFIG:Release>:X:/vcpkg/installed/x64-windows/lib>)

target_link_libraries(${LOCAL_PROJECT_NAME} ${ITK_MODULES})

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER itk/Filtering)
