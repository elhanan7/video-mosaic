#include "image_to_mosaic.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace bpt = boost::property_tree;
using namespace cv;


ImageToMosaic::ImageToMosaic(const bpt::ptree& ini) :
	m_gl(ini), m_tmm(ini), m_ttl(ini), m_ltp(ini), m_pti(ini), m_pts(ini), m_sti(ini)
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
	else
	{
		throw new std::logic_error("Unknown render type: " + tmp);
	}
	m_saveGL = ini.get("ImageToMosaic.SaveGL", true);
	m_saveTopographic = ini.get("ImageToMosaic.SaveTopographic", true);
}


void ImageToMosaic::Process(const cv::Mat_<cv::Vec3b>& input, cv::Mat_<cv::Vec3b>& output)
{
	Mat_<cv::Vec3b> frame, fcolor;
	Mat edges;
	//m_gl.SetCutoff(0.6);

	cv::Mat_<unsigned char> final;
	frame = input;
	
	cvtColor(frame, edges, CV_RGB2GRAY);
	m_gl.Process(edges, edges);
	cv::Mat_<float> dx,dy;
	m_tmm.Process(edges, edges, dx, dy);
	TopographicToLocations::LocationList centers;
	m_ttl.Process(edges, centers);
	std::vector<float> orientations(centers.size());
	std::transform(centers.begin(), centers.end(), orientations.begin(),
		[&,dx,dy](const cv::Point& pt) -> float
	{
		return atan2(dy(pt), dx(pt));
	});
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
	output = fcolor;
}