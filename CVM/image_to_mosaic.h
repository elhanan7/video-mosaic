#pragma once
#include "config.h"

#include <cv.h>
#include <boost/property_tree/ptree_fwd.hpp>

#include "utils.h"
#include "guide_lines.h"
#include "topological_map_maker.h"
#include "topographic_to_locations.h"
#include "topographic_to_locations_iterative.h"
#include "ideal_to_cut_polygons.h"
#include "pov_ray_renderer.h"
#include "polygons_to_scene.h"
#include "scene_to_image.h"
#include "opencv_renderer.h"
#include "voronoi_renderer.h"

namespace videoMosaic
{

class ImageToMosaic
{
 public:
   ImageToMosaic(const boost::property_tree::ptree& ini);
   void Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output,
                cv::Mat motionMask = cv::Mat());
   void Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output, cv::Mat motionMask,
                const cv::Mat& motionTrans);
   cv::Mat GetGuideLinesImage();

 private:
   enum RenderImpl
   {
      RENDER_POV_RAY,
      RENDER_OSG,
      RENDER_OPENCV,
      RENDER_VORONOI
   };

   RenderImpl m_renderImpl;
   bool m_saveTopographic, m_saveMHI, m_saveMotionSegments;

   GuideLines m_guideLines;
   TopologicalMapMaker m_topologicalMapMaker;
   TopographicToLocations m_topologicalToLocations;
   TopographicToLocationsIterative m_iterativePlacer;
   PovRayRenderer m_povRayRenderer;
#ifdef USE_OSG
   PolygonsToScene m_polygonsToScene;
   SceneToImage m_sceneToImage;
#endif
   OpenCVRenderer m_opencvRenderer;
   VoronoiRenderer m_voronoiRenderer;
   PolygonList m_lastPolygons;
   cv::Mat_<unsigned char> m_lastGL;
   cv::Mat_<unsigned char> m_lastTileMask;
   bool m_maskTileLocationsWithMotion;
   bool m_maskGuideLinesWithMotion;
   bool m_recolorize;
   bool m_blurColors;
   cv::Size2f m_tsize;
   cv::Size2f m_origTileSize;
};
}
