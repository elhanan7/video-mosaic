#pragma once

#include <cv.h>
#include <vector>

#include <boost/property_tree/ptree_fwd.hpp>

class TopographicToLocations
{
public:

	TopographicToLocations(const boost::property_tree::ptree& ini);

	typedef std::vector<cv::Point> LocationList;
	void Process(const cv::Mat_<unsigned char>& topo, LocationList& locations);

private:

	int m_tsize;
};




