cmake_minimum_required(VERSION 2.8)

PROJECT( CVM )

#
#Get the definitions from the FindThreads.cmake file which is part of the
#Cmake distribution. This module determines which thread library is available
#on the system
#


file(GLOB SOURCES_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

SET(CVM_HDRS
    ${HEADER_FILES})
	
SET(CVM_SRCS
    ${SOURCES_FILES})

INCLUDE_DIRECTORIES(
  ${CVM_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${Motion_SOURCE_DIR}
  ${OPENSCENEGRAPH_INCLUDE_DIRS}
)

CONFIGURE_FILE( ${CVM_SOURCE_DIR}/config.h.in ${CVM_SOURCE_DIR}/config.h )

ADD_LIBRARY(CVM
  ${CVM_HDRS}
  ${CVM_SRCS}
  )
