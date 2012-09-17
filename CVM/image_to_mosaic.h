#pragma once

#include <cv.h>
#include <boost/property_tree/ptree_fwd.hpp>

#include "guide_lines.h"
#include "topological_map_maker.h"
#include "topographic_to_locations.h"
#include "locations_to_polygons.h"
#include "polygons_to_image.h"
#include "polygons_to_scene.h"
#include "scene_to_image.h"

class ImageToMosaic
{
public:
	ImageToMosaic(const boost::property_tree::ptree& ini);
	void Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output);

private:
	enum RenderImpl
	{
		RENDER_POV_RAY,
		RENDER_OSG
	};

	RenderImpl m_renderImpl;
	bool m_saveGL, m_saveTopographic, m_saveMHI, m_saveMotionSegments;

	GuideLines m_gl;
	TopologicalMapMaker m_tmm;
	TopographicToLocations m_ttl;
	LocationsToPolygons m_ltp;
	PolygonsToImage m_pti;
	PolygonsToScene m_pts;
	SceneToImage m_sti;
};

