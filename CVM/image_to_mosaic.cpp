#include "image_to_mosaic.h"

#include "config.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace bpt = boost::property_tree;
using namespace cv;

namespace videoMosaic {

namespace
{
	cv::Size ProcessTileSize(const bpt::ptree& ini, const cv::Size defval)
	{
		std::string value = ini.get("ImageToMosaic.TileSize", "");
		if (value == "")
		{
			return defval;
		}
		boost::algorithm::trim(value);
		std::vector<std::string> tokens;
		boost::algorithm::split(tokens, value, boost::is_any_of(",x "), boost::token_compress_on);
		if (tokens.size() == 0)
		{
			return defval;
		}
		else if (tokens.size() == 1)
		{
			int size = boost::lexical_cast<int>(tokens[0]);
			return cv::Size(size,size);
		}
		else
		{
			int sizex = boost::lexical_cast<int>(tokens[0]);
			int sizey = boost::lexical_cast<int>(tokens[1]);
			return cv::Size(sizex,sizey);
		}
	}

	void JoinPolygonsWithMask(const IdealPolygonList& oldPolys,
							  const IdealPolygonList& newPolys,
							  const cv::Mat_<unsigned char> mask,
							  IdealPolygonList& result)
	{
		for (auto iter = oldPolys.cbegin(); iter != oldPolys.cend(); ++iter)
		{
			if (!mask(iter->center))
			{
				result.push_back(*iter);
			}
		}

		for (auto iter = newPolys.cbegin(); iter != newPolys.cend(); ++iter)
		{
			if (mask(iter->center))
			{
				result.push_back(*iter);
			}
		}
	}
}

ImageToMosaic::ImageToMosaic(const bpt::ptree& ini) :
	m_guideLines(ini), m_topologicalMapMaker(ini), m_topologicalToLocations(ini), 
	m_idealToCutPolygon(ini), m_povRayRenderer(ini), 
#ifdef USE_OSG
	m_polygonsToScene(ini), m_sceneToImage(ini), 
#endif
	m_opencvRenderer(ini), m_voronoiRenderer(ini)
{
	std::string tmp;
	tmp = ini.get("ImageToMosaic.RenderImpl", "POV_RAY");
	boost::algorithm::to_lower(tmp);
	if (tmp == "pov_ray")
	{
		m_renderImpl = RENDER_POV_RAY;
	}
#ifdef USE_OSG
	else if (tmp == "osg")
	{
		m_renderImpl = RENDER_OSG;
	}
#endif
	else if (tmp == "opencv")
	{
		m_renderImpl = RENDER_OPENCV;
	}
	else if (tmp == "voronoi")
	{
		m_renderImpl = RENDER_VORONOI;
	}
	else
	{
		throw new std::logic_error("Unknown render type: " + tmp);
	}
	m_saveTopographic = ini.get("ImageToMosaic.SaveTopographic", true);
	m_maskTileLocationsWithMotion = ini.get("ImageToMosaic.MaskTileLocationsWithMotion", false);
	m_maskGuideLinesWithMotion = ini.get("ImageToMosaic.MaskGuideLinesWithMotion", true);
	m_tsize = ProcessTileSize(ini, cv::Size(4,4));
	m_recolorize = ini.get("ImageToMosaic.RecolorizeOnEachFrame", true);
}


void ImageToMosaic::Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output, cv::Mat motionMask)
{
	Mat_<cv::Vec3b> frame, fcolor;
	Mat edges;

	cv::Mat_<unsigned char> final;
	frame = input;
	
	cvtColor(frame, edges, CV_RGB2GRAY);
	m_guideLines.Process(edges, edges);
	if (m_maskGuideLinesWithMotion && !motionMask.empty())
	{
		m_lastGL.copyTo(edges, 1 - motionMask);
	}
	m_lastGL = edges.clone();
	cv::Mat_<float> dx,dy;
	m_topologicalMapMaker.Process(edges, edges, m_tsize, dx, dy);
	IdealPolygonList currentPolygons, polygons;
	m_topologicalToLocations.Process(edges, dx, dy, m_tsize, currentPolygons);
	
	for (auto iter = currentPolygons.begin(); iter != currentPolygons.end(); ++iter)
	{
		iter->color = input(iter->center);
		iter->orientation = atan2(dy(iter->center), dx(iter->center));
	}
	
	if (m_maskTileLocationsWithMotion &&  !motionMask.empty())
	{
		JoinPolygonsWithMask(m_lastPolygons, currentPolygons, motionMask, polygons);
	}
	else
	{
		polygons = currentPolygons;
	}

	if (m_recolorize)
	{
		for (auto iter = polygons.begin(); iter != polygons.end(); ++iter)
		{
			iter->color = input(iter->center);
		}
	}
	m_lastPolygons = polygons;


	if (m_renderImpl == RENDER_VORONOI)
	{
		m_voronoiRenderer.Process(polygons, input.size(), fcolor);
		output = fcolor;
		return;
	}
	
	PolygonList cutPolys;

	m_idealToCutPolygon.Process(polygons, m_tsize, frame.size(), cutPolys);
	if (m_renderImpl == RENDER_POV_RAY)
	{
		m_povRayRenderer.Process(cutPolys, frame.size(), fcolor);
	}
#ifdef USE_OSG
	else if (m_renderImpl == RENDER_OSG)
	{
		osg::Node* scene = m_polygonsToScene.Process(cutPolys);
		m_sceneToImage.Process(scene,cv::Size(frame.cols, frame.rows) ,fcolor);
	}
#endif
	else if (m_renderImpl == RENDER_OPENCV)
	{
		m_opencvRenderer.Process(cutPolys, frame.size(), fcolor);
	}

	output = fcolor;
}

}
