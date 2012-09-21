#include "image_to_mosaic.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace bpt = boost::property_tree;
using namespace cv;

namespace
{
	void JoinLocationsWithMask(const TopographicToLocations::LocationList& oldLocs,
							const TopographicToLocations::LocationList& newLocs,
							const cv::Mat_<unsigned char> mask,
							TopographicToLocations::LocationList& result)
	{
		for (auto iter = oldLocs.cbegin(); iter != oldLocs.cend(); ++iter)
		{
			if (!mask(*iter))
			{
				result.push_back(*iter);
			}
		}

		for (auto iter = newLocs.cbegin(); iter != newLocs.cend(); ++iter)
		{
			if (mask(*iter))
			{
				result.push_back(*iter);
			}
		}
	}
}

ImageToMosaic::ImageToMosaic(const bpt::ptree& ini) :
	m_gl(ini), m_tmm(ini), m_ttl(ini), m_ltp(ini), m_pti(ini), m_pts(ini), m_sti(ini), m_ocvRender(ini)
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
	else if (tmp == "superpixel")
	{
		m_renderImpl = RENDER_SUPERPIXEL;
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
	m_gl.Process(edges, edges);
	if (m_maskGuideLinesWithMotion && !motionMask.empty())
	{
		m_lastGL.copyTo(edges, 1 - motionMask);
	}
	m_lastGL = edges.clone();
	cv::Mat_<float> dx,dy;
	m_tmm.Process(edges, edges, dx, dy);
	TopographicToLocations::LocationList currentCenters, centers;
	m_ttl.Process(edges, currentCenters);
	if (m_maskTileLocationsWithMotion &&  !motionMask.empty())
	{
		JoinLocationsWithMask(m_lastLocations, currentCenters, motionMask, centers);
	}
	else
	{
		centers = currentCenters;
	}
	m_lastLocations = centers;
	std::vector<float> orientations(centers.size());
	std::transform(centers.begin(), centers.end(), orientations.begin(),
		[&,dx,dy](const cv::Point& pt) -> float
	{
		return atan2(dy(pt), dx(pt));
	});
	if (m_renderImpl == RENDER_SUPERPIXEL)
	{
		m_spr.Process(centers, frame, fcolor);
		output = fcolor;
		return;
	}
	LocationsToPolygons::PolygonList polys;

	m_ltp.Process(centers, orientations, cv::Size(frame.cols, frame.rows), polys);
	if (m_renderImpl == RENDER_POV_RAY)
	{
		m_pti.Process(frame, polys, fcolor);
	}
	else if (m_renderImpl == RENDER_OSG)
	{
		osg::Node* scene = m_pts.Process(frame, polys);
		m_sti.Process(scene,cv::Size(frame.cols, frame.rows) ,fcolor);
	}
	else if (m_renderImpl == RENDER_OPENCV)
	{
		m_ocvRender.Process(frame, polys, fcolor);
	}

	output = fcolor;
}
