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
	cv::Size2f ProcessTileSize(const bpt::ptree& ini, const cv::Size2f defval)
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
			float size = boost::lexical_cast<float>(tokens[0]);
			return cv::Size2f(size,size);
		}
		else
		{
			float sizex = boost::lexical_cast<float>(tokens[0]);
			float sizey = boost::lexical_cast<float>(tokens[1]);
			return cv::Size2f(sizex,sizey);
		}
	}

	void JoinPolygonsWithMask(const PolygonList& oldPolys,
							  const PolygonList& newPolys,
							  const cv::Mat_<unsigned char> motionMask,
							  const cv::Mat_<unsigned char> tilesMask,
							  PolygonList& result)
	{
		for (auto iter = oldPolys.cbegin(); iter != oldPolys.cend(); ++iter)
		{
			if (!motionMask(iter->center))
			{
				result.push_back(*iter);
			}
		}

		for (auto iter = newPolys.cbegin(); iter != newPolys.cend(); ++iter)
		{
			result.push_back(*iter);
		}
	}

	void FixTileMask(const PolygonList& oldPolys,
					 const cv::Mat_<unsigned char> motionMask,
					 const cv::Mat_<unsigned char> tilesMask)
	{
		for (auto iter = oldPolys.cbegin(); iter != oldPolys.cend(); ++iter)
		{
			if (motionMask(iter->center))
			{
				std::vector<cv::Point> intPoly;
				std::transform(iter->vertices.cbegin(), iter->vertices.cend(), std::back_inserter(intPoly), utils::ToIntPoint);
				cv::fillConvexPoly(tilesMask, intPoly, 0);
			}
		}
	}
}

ImageToMosaic::ImageToMosaic(const bpt::ptree& ini) :
	m_guideLines(ini), m_topologicalMapMaker(ini), m_topologicalToLocations(ini), 
	m_povRayRenderer(ini), 
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
	m_origTileSize = ProcessTileSize(ini, cv::Size2f(4,4));
	m_tsize = m_origTileSize;
	m_recolorize = ini.get("ImageToMosaic.RecolorizeOnEachFrame", true);
	m_blurColors = ini.get("ImageToMosaic.BlurBeforeColorization", false);
}


void ImageToMosaic::Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output, cv::Mat motionMask)
{
	Mat_<cv::Vec3b> frame, fcolor;
	Mat edges;

	cv::Mat_<unsigned char> final;
	frame = input;
	
	cvtColor(frame, edges, CV_RGB2GRAY);
	if (motionMask.empty())
	{
		m_tsize = m_origTileSize;
	}
	m_guideLines.Process(edges, edges);
	if (m_maskGuideLinesWithMotion && !motionMask.empty())
	{
		m_lastGL.copyTo(edges, 1 - motionMask);
	}
	m_lastGL = edges.clone();
	cv::Mat_<float> dx,dy;
	m_topologicalMapMaker.Process(edges, edges, m_tsize, dx, dy);
	PolygonList currentPolygons, polygons;
	if (m_maskTileLocationsWithMotion &&  !motionMask.empty())
	{
		m_lastTileMask.setTo(0, motionMask);
		FixTileMask(m_lastPolygons, motionMask, m_lastTileMask);
	}
	else
	{
		m_lastTileMask.create(input.size());
		m_lastTileMask.setTo(0);
	}
	m_topologicalToLocations.Process(edges, dx, dy, m_tsize, m_lastTileMask, currentPolygons);
	
	for (auto iter = currentPolygons.begin(); iter != currentPolygons.end(); ++iter)
	{
		iter->color = input(iter->center);
		iter->orientation = atan2(dy(iter->center), dx(iter->center));
	}
	
	if (m_maskTileLocationsWithMotion &&  !motionMask.empty())
	{
		JoinPolygonsWithMask(m_lastPolygons, currentPolygons, motionMask, m_lastTileMask, polygons);
	}
	else
	{
		polygons = currentPolygons;
	}

	if (m_recolorize)
	{
		cv::Mat_<cv::Vec3b> blurred;
		if (m_blurColors)
		{
			cv::Size kerSize(static_cast<int>(m_tsize.width * 1.5f) / 2 * 2 + 1,
			static_cast<int>(m_tsize.height * 1.5f) / 2 * 2 + 1);
			cv::GaussianBlur(input, blurred, kerSize, 2);
		}
		else
		{
			blurred = input;
		}
		for (auto iter = polygons.begin(); iter != polygons.end(); ++iter)
		{
			iter->color = blurred(clamp(iter->center, blurred.size()));
		}
	}
	m_lastPolygons = polygons;

	if (m_renderImpl == RENDER_VORONOI)
	{
		m_voronoiRenderer.Process(polygons, input.size(), fcolor);
		output = fcolor;
		return;
	}
	
	if (m_renderImpl == RENDER_POV_RAY)
	{
		m_povRayRenderer.Process(polygons, frame.size(), fcolor);
	}
#ifdef USE_OSG
	else if (m_renderImpl == RENDER_OSG)
	{
		osg::Node* scene = m_polygonsToScene.Process(polygons);
		m_sceneToImage.Process(scene,cv::Size(frame.cols, frame.rows) ,fcolor);
	}
#endif
	else if (m_renderImpl == RENDER_OPENCV)
	{
		m_opencvRenderer.Process(polygons, frame.size(), fcolor);
	}

	output = fcolor;
}

void ImageToMosaic::Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output, cv::Mat motionMask, const cv::Mat& motionTrans)
{
	cv::Mat_<unsigned char> tmp;
	cv::warpPerspective(m_lastGL, tmp, motionTrans, m_lastGL.size(), cv::INTER_NEAREST);
	m_lastGL = tmp;
	cv::warpPerspective(m_lastTileMask, tmp, motionTrans, m_lastTileMask.size(), cv::INTER_NEAREST);
	m_lastTileMask = tmp;

	for (auto iter = m_lastPolygons.begin(); iter != m_lastPolygons.end(); ++iter)
	{
		cv::perspectiveTransform(iter->vertices, iter->vertices, motionTrans);
		std::vector<cv::Point2d> centerVectorized;
		centerVectorized.push_back(iter->center);
		cv::perspectiveTransform(centerVectorized, centerVectorized, motionTrans);
		iter->center = clamp(centerVectorized[0], input.size());
	}
	Process(input, output, motionMask);
}

}
