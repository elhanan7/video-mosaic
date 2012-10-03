cmake_minimum_required(VERSION 2.8)

PROJECT( CVM )

#
#Get the definitions from the FindThreads.cmake file which is part of the
#Cmake distribution. This module determines which thread library is available
#on the system
#



SET(CVM_HDRS
    adapt_cv_point.h
	directory_source.h
	guide_lines.h
	ideal_to_cut_polygons.h
	image_to_mosaic.h
	locator_grid.h
	opencv_renderer.h
	polygons_to_scene.h
	pov_ray_renderer.h
	scene_to_image.h
	topographic_to_locations.h
	topological_map_maker.h
	transfromations.h
	utils.h
	video_source.h
	video_to_mosaic.h
	voronoi_renderer.h
	approximate_tile_placer.h
	precise_tile_placer.h)
	
SET(CVM_SRCS
	clipper.cpp
	directory_source.cpp
	guide_lines.cpp
	ideal_to_cut_polygons.cpp
	image_to_mosaic.cpp
	offscreen_render.cpp
	opencv_renderer.cpp
	polygons_to_scene.cpp
	pov_ray_renderer.cpp
	scene_to_image.cpp
	topographic_to_locations.cpp
	topological_map_maker.cpp
	video_to_mosaic.cpp
	voronoi_renderer.cpp)



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
