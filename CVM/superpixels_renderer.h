#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <cv.h>

class SuperpixelsRenderer
{
public:
	SuperpixelsRenderer(const boost::property_tree::ptree& ini);

	void Process(const std::vector<cv::Point>& locations, cv::Mat_<cv::Vec3b> colorImage, cv::Mat& output);

	int m_sizeMultiplier;
};

