cmake_minimum_required(VERSION 2.8)

PROJECT( process_file )

INCLUDE_DIRECTORIES (
  ${CVM_SOURCE_DIR}
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
)

LINK_DIRECTORIES(${CVM_BINARY_DIR}
				 ${Motion_BINARY_DIR}
				 ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(process_file process_file.cpp)
  
TARGET_LINK_LIBRARIES(process_file CVM Motion ${Boost_LIBRARIES} ${OpenCV_LIBS} ${OPENSCENEGRAPH_LIBRARIES})

