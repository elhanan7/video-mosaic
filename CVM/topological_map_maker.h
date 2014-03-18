#pragma once

#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

namespace videoMosaic {

class TopologicalMapMaker
{
public:
	TopologicalMapMaker(const boost::property_tree::ptree& ini);
	void Process(const cv::Mat& in, cv::Mat& out, cv::Size2f tsize, cv::Mat& distance, cv::Mat& dx, cv::Mat& dy);
};

}