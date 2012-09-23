#include "image_to_mosaic.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace bpt = boost::property_tree;
using namespace cv;

namespace
{
	void JoinLocationsWithMask(const TopographicToLocations::LocationList& oldLocs,
							const std::vector<float> oldOrientations,
							const TopographicToLocations::LocationList& newLocs,
							const std::vector<float> newOrientations,
							const cv::Mat_<unsigned char> mask,
							TopographicToLocations::LocationList& result,
							std::vector<float>& resultOrientation)
	{
		size_t idx = 0;
		for (auto iter = oldLocs.cbegin(); iter != oldLocs.cend(); ++iter)
		{
			if (!mask(*iter))
			{
				result.push_back(*iter);
				resultOrientation.push_back(oldOrientations[idx]);
			}
			++idx;
		}

		idx = 0;
		for (auto iter = newLocs.cbegin(); iter != newLocs.cend(); ++iter)
		{
			if (mask(*iter))
			{
				result.push_back(*iter);
				resultOrientation.push_back(newOrientations[idx]);
			}
			++idx;
		}
	}
}

ImageToMosaic::ImageToMosaic(const bpt::ptree& ini) :
	m_guideLines(ini), m_topologicalMapMaker(ini), m_topologicalToLocations(ini), m_locationsToPolygons(ini), m_povRayRenderer(ini), m_polygonsToScene(ini), m_sceneToImage(ini), m_opencvRenderer(ini), m_voronoiRenderer(ini)
{
	std::string tmp;
	tmp = ini.get("ImageToMosaic.RenderImpl", "POV_RAY");
	boost::algorithm::to_lower(tmp);
	if (tmp == "pov_ray")
	{
		m_renderImpl = RENDER_POV_RAY;
	}
	else if (tmp == "osg")
	{
		m_renderImpl = RENDER_OSG;
	}
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
	m_topologicalMapMaker.Process(edges, edges, dx, dy);
	TopographicToLocations::LocationList currentCenters, centers;
	m_topologicalToLocations.Process(edges, currentCenters);
	std::vector<float> currentOrientations(currentCenters.size()), orientations;
	std::transform(currentCenters.begin(), currentCenters.end(), currentOrientations.begin(),
		[&,dx,dy](const cv::Point& pt) -> float
	{
		return atan2(dy(pt), dx(pt));
	});
	if (m_maskTileLocationsWithMotion &&  !motionMask.empty())
	{
		JoinLocationsWithMask(m_lastLocations, m_lastOrientations, currentCenters, currentOrientations, motionMask, centers, orientations);
	}
	else
	{
		centers = currentCenters;
		orientations = currentOrientations;
	}
	m_lastLocations = centers;
	m_lastOrientations = orientations;
	if (m_renderImpl == RENDER_VORONOI)
	{
		m_voronoiRenderer.Process(centers, frame, fcolor);
		output = fcolor;
		return;
	}
	LocationsToPolygons::PolygonList polys;

	m_locationsToPolygons.Process(centers, orientations, cv::Size(frame.cols, frame.rows), polys);
	if (m_renderImpl == RENDER_POV_RAY)
	{
		m_povRayRenderer.Process(frame, polys, fcolor);
	}
	else if (m_renderImpl == RENDER_OSG)
	{
		osg::Node* scene = m_polygonsToScene.Process(frame, polys);
		m_sceneToImage.Process(scene,cv::Size(frame.cols, frame.rows) ,fcolor);
	}
	else if (m_renderImpl == RENDER_OPENCV)
	{
		m_opencvRenderer.Process(frame, polys, fcolor);
	}

	output = fcolor;
}
