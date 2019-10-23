myproject(OpenCVViewImage0)

find_package(OpenCV REQUIRED )

add_executable(${LOCAL_PROJECT_NAME} MACOSX_BUNDLE src/opencvViewImage0.cxx  Include/opencvViewImage0.h)

target_link_libraries(${LOCAL_PROJECT_NAME} 
	PRIVATE 
      opencv_core 
      opencv_video
      opencv_imgproc
	  opencv_highgui
)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER opencv/Images)
