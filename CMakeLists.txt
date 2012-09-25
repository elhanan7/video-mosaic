cmake_minimum_required(VERSION 2.8)

PROJECT (VideoMosaic)
OPTION(USE_OSG "Use OpenSceneGraph" ON)
IF (USE_OSG)
SET (OSG_DIR "NOTFOUND" CACHE DIRPATH "Location of OSG Tree")
ENDIF(USE_OSG)

SET (BOOST_ROOT "NOTFOUND" CACHE DIRPATH "Location of Boost Tree")
FIND_PACKAGE( Boost 1.40 COMPONENTS regex system filesystem REQUIRED )
FIND_PACKAGE( OpenCV REQUIRED )

IF (USE_OSG)
    set(ENV{OSG_DIR} ${OSG_DIR})
	find_package(OpenSceneGraph 3.0.0 REQUIRED osgGA osgViewer osgUtil osgDB)
	IF(OSG_FOUND)
		INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR})
	ENDIF(OSG_FOUND)
	SET (OSG_LIBS osgd.lib OpenThreadsd.lib osgGAd.lib osgViewerd.lib osgUtild.lib osgDBd.lib opengl32.lib)
ELSE (USE_OSG)
	SET(USE_OSG OFF)
ENDIF (USE_OSG)

ADD_SUBDIRECTORY(Motion)
ADD_SUBDIRECTORY(CVM)
ADD_SUBDIRECTORY(process_file)
ADD_SUBDIRECTORY(process_directory)

