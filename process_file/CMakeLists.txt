cmake_minimum_required(VERSION 2.8)

PROJECT( process_file )

INCLUDE_DIRECTORIES (
  ${CVM_SOURCE_DIR}
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${OPENSCENEGRAPH_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${CVM_BINARY_DIR}
				 ${Motion_BINARY_DIR}
				 ${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(process_file process_file.cpp)

add_custom_command(TARGET process_file
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CVM_SOURCE_DIR}/cvm.ini ${process_file_BINARY_DIR})
  
TARGET_LINK_LIBRARIES(process_file CVM Motion ${Boost_LIBRARIES} ${OpenCV_LIBS} ${OPENSCENEGRAPH_LIBRARIES})

