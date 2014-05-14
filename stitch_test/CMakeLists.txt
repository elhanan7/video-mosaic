cmake_minimum_required(VERSION 2.8)

PROJECT( stitch_test )

INCLUDE_DIRECTORIES (
  ${CVM_SOURCE_DIR}
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${OPENSCENEGRAPH_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${CVM_BINARY_DIR}
				 ${Motion_BINARY_DIR}
				 ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(stitch_test stitch_test.cpp)

TARGET_LINK_LIBRARIES(stitch_test CVM Motion ${Boost_LIBRARIES} ${OpenCV_LIBS} ${OPENSCENEGRAPH_LIBRARIES})

