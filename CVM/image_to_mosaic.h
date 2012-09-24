#pragma once

#include <cv.h>
#include <boost/property_tree/ptree_fwd.hpp>

#include "utils.h"
#include "guide_lines.h"
#include "topological_map_maker.h"
#include "topographic_to_locations.h"
#include "ideal_to_cut_polygons.h"
#include "pov_ray_renderer.h"
#include "polygons_to_scene.h"
#include "scene_to_image.h"
#include "opencv_renderer.h"
#include "voronoi_renderer.h"

namespace videoMosaic {

class ImageToMosaic
{
public:
	ImageToMosaic(const boost::property_tree::ptree& ini);
	void Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output, cv::Mat motionMask = cv::Mat());

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
	IdealToCutPolygon m_idealToCutPolygon;
	PovRayRenderer m_povRayRenderer;
	PolygonsToScene m_polygonsToScene;
	SceneToImage m_sceneToImage;
	OpenCVRenderer m_opencvRenderer;
	VoronoiRenderer m_voronoiRenderer;
	IdealPolygonList m_lastPolygons;
	cv::Mat_<unsigned char> m_lastGL;
	bool m_maskTileLocationsWithMotion;
	bool m_maskGuideLinesWithMotion;
	cv::Size m_tsize;
};

}

