cmake_minimum_required(VERSION 2.8)

PROJECT( Motion )

SET(Motion_HDRS
    video_to_mhi.h
	global_motion_estimator.h)
	
SET(Motion_SRCS
	video_to_mhi.cpp
	global_motion_estimator.cpp)



INCLUDE_DIRECTORIES(
  ${Motion_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIR}
)

ADD_LIBRARY(Motion
  ${Motion_HDRS}
  ${Motion_SRCS}
  )
