#pragma once

#include <cv.h>
#include <vector>

#include <boost/property_tree/ptree_fwd.hpp>

#include "utils.h"

namespace videoMosaic {

class TopographicToLocations
{
public:

	TopographicToLocations(const boost::property_tree::ptree& ini);

	void Process(const cv::Mat_<unsigned char>& topo, cv::Size tsize, IdealPolygonList& polygons);

private:

	int m_holeFillingIterations;
};

}


