#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <cv.h>

namespace videoMosaic {

class GlobalMotionEstimator
{
public:
	GlobalMotionEstimator(const boost::property_tree::ptree& ini);
	bool Estimate(cv::Mat_<unsigned char> from, cv::Mat_<unsigned char> to, cv::Mat& trans);
	static void CalculateValidMask(cv::Mat trans, const cv::Size& sz, cv::Mat& mask);
};

}


