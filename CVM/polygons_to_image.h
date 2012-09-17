#pragma once

#include <cv.h>
#include <boost/property_tree/ptree_fwd.hpp>

class PolygonsToImage
{
public:
	typedef std::vector<cv::Point2d> Polygon;
	typedef std::vector<Polygon> PolygonList;

	PolygonsToImage(const boost::property_tree::ptree& ini);

	void Process(const cv::Mat_<cv::Vec3b>& img, const PolygonList& polygons, cv::Mat& res);

private:
	int m_sizeMultiplier;
	std::string m_povrayPath;
	std::string m_povIniPath;
};

