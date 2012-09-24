#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <cv.h>

#include "utils.h"

namespace videoMosaic {

class VoronoiRenderer
{
public:
	VoronoiRenderer(const boost::property_tree::ptree& ini);

	void Process(const IdealPolygonList& polys, cv::Size sz, cv::Mat& output);

	int m_sizeMultiplier;
};

}