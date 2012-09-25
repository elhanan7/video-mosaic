cmake_minimum_required(VERSION 2.8)

PROJECT( process_directory )

INCLUDE_DIRECTORIES (
  ${CVM_SOURCE_DIR}
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
)

LINK_DIRECTORIES(${CVM_BINARY_DIR}
				 ${Motion_BINARY_DIR}
				 ${Boost_LIBRARY_DIRS})
				 
IF (USE_OSG)
   LINK_DIRECTORIES(${OSG_DIR}/lib)
   
ENDIF (USE_OSG)

list (REMOVE_ITEM OpenCV_LIBS opencv_ts242d.lib)
SET (MY_OPENCV_LIBS ${OpenCV_LIBS})

ADD_EXECUTABLE(process_directory process_directory.cpp)
  
TARGET_LINK_LIBRARIES(process_directory CVM Motion ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${OSG_LIBS} ${MY_OPENCV_LIBS})

