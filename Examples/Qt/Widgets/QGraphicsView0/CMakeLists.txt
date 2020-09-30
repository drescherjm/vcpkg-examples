myproject(QGraphicsView0)

# set your files and resources here  
set(${LOCAL_PROJECT_NAME}_SRCS 
	./src/main.cxx
	./src/QGraphicsView0.cxx
)

set(${LOCAL_PROJECT_NAME}_UIS 
	./rc/QGraphicsView0.ui
)

set(${LOCAL_PROJECT_NAME}_HDRS
	
)

set(${LOCAL_PROJECT_NAME}_MOC_HDRS
	./Include/QGraphicsView0.h
)

# and finally an resource file
set( ${LOCAL_PROJECT_NAME}_RSC 
)
 
setup_qt_executable(${LOCAL_PROJECT_NAME})
 
SOURCE_GROUP("Resources" FILES
  ${${LOCAL_PROJECT_NAME}_UI}
  ${${LOCAL_PROJECT_NAME}_RSC}
  ${${LOCAL_PROJECT_NAME}_SCRIPTS}
  ${EXE_ICON}
)
 
SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${ResourceSrcs}
  ${RCS_SOURCES}
)

INCLUDE_DIRECTORIES(Include
	${LOCAL_PROJECT_BINARY_DIR}
	)
  
TARGET_LINK_LIBRARIES( ${LOCAL_PROJECT_NAME} ${QT_LIBRARIES} ${UPMC_EXTERNAL_LIBS} )

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER Qt/GUI)

