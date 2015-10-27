cmake_minimum_required(VERSION 2.8)

PROJECT( process_scene )

INCLUDE_DIRECTORIES (
  ${CVM_SOURCE_DIR}
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${OPENSCENEGRAPH_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${CVM_BINARY_DIR}
				 ${Motion_BINARY_DIR}
				 ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(process_scene process_scene.cpp)

TARGET_LINK_LIBRARIES(process_scene CVM Motion ${Boost_LIBRARIES} ${OpenCV_LIBS} ${OPENSCENEGRAPH_LIBRARIES})

