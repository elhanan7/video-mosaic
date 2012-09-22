#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>
#include <cv.h>

class OpenCVRenderer
{
public:
	OpenCVRenderer(const boost::property_tree::ptree& ini);

	typedef std::vector<cv::Point2d> Polygon;
	typedef std::vector<Polygon> PolygonList;

	void Process(const cv::Mat_<cv::Vec3b>& colorImage, const PolygonList& polygons, cv::Mat& output);

private:
	int m_sizeMultiplier;
};

