myproject(CADe)

find_package(ITK REQUIRED )
include(${ITK_USE_FILE})

find_package(CLI11 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

################################################################################

set (LOCAL_PROJECT_NAME_SOURCE_FILES
	src/CADe_main.cxx  
	src/CADe.cxx
	src/CADe_Private.cxx
)

set (LOCAL_PROJECT_NAME_HEADER_FILES
	 Include/CADe_main.h 
	 Include/CADe.h
	 Include/CADe_Private.h
	 Include/CADe_Score.h
)

################################################################################

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE ${LOCAL_PROJECT_NAME_SOURCE_FILES} ${LOCAL_PROJECT_NAME_HEADER_FILES})

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE Include)

set(ITK_MODULES ITKSpatialObjects ITKCommon ITKIOImageBase ITKIOBMP ITKIOGDCM ITKIOPNG ITKIOJPEG ITKIOJPEG2000 ITKIOTIFF ITKIOMINC ITKIOMRC ITKIONIFTI ITKIONRRD ITKIOStimulate ITKIOMeta ITKIOBioRad ITKIOBruker ITKIOGIPL ITKIOHDF5 ITKIOGE ITKIOLSM ITKIOVTK ITKVTK)

if (ITKVtkGlue_LOADED) 
	set(ITK_MODULES ITKVtkGlue ${ITK_MODULES})
	
	target_compile_definitions( ${LOCAL_PROJECT_NAME} PRIVATE ENABLE_QUICKVIEW)
endif()

target_link_directories(${LOCAL_PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:X:/vcpkg/installed/x64-windows/debug/lib> $<$<CONFIG:Release>:X:/vcpkg/installed/x64-windows/lib>)

target_link_libraries(${LOCAL_PROJECT_NAME} ${ITK_MODULES} CLI11::CLI11)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER itk/Filtering)
