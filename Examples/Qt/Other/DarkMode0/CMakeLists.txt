myproject(DarkMode0)

set (QT_LIBRARIES Qt5::Core Qt5::Network Qt5::Sql Qt5::Widgets Qt5::Gui Qt5::WinMain Qt5::Xml Qt5::PrintSupport)
set (QT_MODULES Qt5::Core Qt5::Network Qt5::Sql Qt5::Widgets Qt5::Gui Qt5::WinMain Qt5::Xml Qt5::PrintSupport)
set (QT_PLATFORM_MODULES Qt5::QWindowsIntegrationPlugin)
find_qt5_packages()

# set your files and resources here  
set(${LOCAL_PROJECT_NAME}_SRCS 
	./src/main.cxx
	./src/DM0Dialog.cxx
)

set(${LOCAL_PROJECT_NAME}_UIS 
	./rc/DM0Dialog.ui
)

set(${LOCAL_PROJECT_NAME}_HDRS
	
)

set(${LOCAL_PROJECT_NAME}_MOC_HDRS
	./Include/DM0Dialog.h
)

# and finally an resource file
set( ${LOCAL_PROJECT_NAME}_RCS 
	
)
 
setup_qt_executable(${LOCAL_PROJECT_NAME})
 
SOURCE_GROUP("Resources" FILES
  ${${LOCAL_PROJECT_NAME}_UI}
  ${${LOCAL_PROJECT_NAME}_RCS}
  ${${LOCAL_PROJECT_NAME}_SCRIPTS}
  ${EXE_ICON}
)
 
SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${ResourceSrcs}
  ${RCS_SOURCES}
)

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE ${LOCAL_PROJECT_SOURCE_DIR}/Include
	${LOCAL_PROJECT_BINARY_DIR}
	)
  
TARGET_LINK_LIBRARIES( ${LOCAL_PROJECT_NAME} ${QT_LIBRARIES} ${UPMC_EXTERNAL_LIBS} )

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER Qt/GUI)

