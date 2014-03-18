cmake_minimum_required(VERSION 2.8)

PROJECT (VideoMosaic)
OPTION(USE_OSG "Use OpenSceneGraph" ON)
IF (USE_OSG)
SET (OSG_DIR "NOTFOUND" CACHE PATH "Location of OSG Tree")
ENDIF(USE_OSG)

SET (BOOST_ROOT "NOTFOUND" CACHE PATH "Location of Boost Tree")
FIND_PACKAGE( Boost 1.40 COMPONENTS regex system filesystem REQUIRED python)
FIND_PACKAGE( OpenCV REQUIRED )

IF (USE_OSG)
	find_package(OpenSceneGraph 3.0.0 REQUIRED osgGA osgViewer osgUtil osgDB)
	
	IF (MSVC)
		LIST (APPEND OPENSCENEGRAPH_LIBRARIES opengl32.lib)
	ENDIF(MSVC)
ENDIF (USE_OSG)

IF (NOT OPENSCENEGRAPH_FOUND)
	SET (USE_OSG OFF CACHE OPTION "Use OpenSceneGraph")
ENDIF()

IF (UNIX)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC")
ENDIF()

ADD_SUBDIRECTORY(Motion)
ADD_SUBDIRECTORY(CVM)
ADD_SUBDIRECTORY(process_file)
ADD_SUBDIRECTORY(process_directory)
ADD_SUBDIRECTORY(python_video_mosaic)

