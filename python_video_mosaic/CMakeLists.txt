cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(PythonLibs)

IF (PYTHONLIBS_FOUND)
INCLUDE(FindNumpy.cmake)
IF (NUMPY_FOUND)
PROJECT( python_video_mosaic )

INCLUDE_DIRECTORIES (
  ${CVM_SOURCE_DIR}
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${OPENSCENEGRAPH_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_PATH}
  ${NUMPY_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${CVM_BINARY_DIR}
				 ${Motion_BINARY_DIR}
				 ${Boost_LIBRARY_DIRS})
				 
SET (PYTHON_VIDEO_MOSAIC_HDRS cvm_python_classes.h)
SET (PYTHON_VIDEO_MOSAIC_SRCS module.cpp cvm_python_classes.cpp numpy_to_cv.cpp)
ADD_LIBRARY(video_mosaic SHARED ${PYTHON_VIDEO_MOSAIC_HDRS} ${PYTHON_VIDEO_MOSAIC_SRCS})

set_target_properties(video_mosaic
                      PROPERTIES 
					  PROJECT_LABEL python_video_mosaic
					  PREFIX "")

IF (MSVC)
set_target_properties(video_mosaic
                      PROPERTIES 
					  SUFFIX .pyd)
ENDIF (MSVC)
  
TARGET_LINK_LIBRARIES(video_mosaic CVM Motion ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES} ${OpenCV_LIBS} ${OPENSCENEGRAPH_LIBRARIES})
ENDIF(NUMPY_FOUND)
ENDIF(PYTHONLIBS_FOUND)

