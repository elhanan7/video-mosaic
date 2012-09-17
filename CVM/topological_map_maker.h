#pragma once

#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

class TopologicalMapMaker
{
public:
	TopologicalMapMaker(const boost::property_tree::ptree& ini);
	void Process(const cv::Mat& in, cv::Mat& out, cv::Mat& dx, cv::Mat& dy);
private:
	int m_tsize;
};

