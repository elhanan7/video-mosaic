#pragma once

#include <cv.h>
#include <vector>

#include "utils.h"

#include <boost/property_tree/ptree_fwd.hpp>

namespace videoMosaic {

class IdealToCutPolygon
{
public:
	IdealToCutPolygon(const boost::property_tree::ptree& ini);

	void Process(const PolygonList& ideal, const cv::Size2f& tsize, const cv::Size& imsize, PolygonList& polygons);
};
}