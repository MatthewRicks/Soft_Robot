cmake_minimum_required(VERSION 3.4)
project(SoftRobotModel)


FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)
FIND_PACKAGE(OpenSceneGraph REQUIRED COMPONENTS osgDB osgGA osgUtil osgViewer osgText)

INCLUDE_DIRECTORIES( ${OPENSCENEGRAPH_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)


SET(MYSOURCE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    xmlreader.cpp
    xmlreader.h
    xmlwriter.cpp
    xmlwriter.h
    joint.h
    joint.cpp
	osgwidget.h
	osgwidget.cpp
        inputwindow.h
        inputwindow.cpp
        inputwindow.ui
    )
add_executable(${PROJECT_NAME}
    ${MYSOURCE}
    )


target_link_libraries(${PROJECT_NAME}
    ${OPENSCENEGRAPH_LIBRARIES}	
    Qt5::Widgets
    Qt5::Gui
)
