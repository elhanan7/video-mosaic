#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>
#include <cv.h>

#include "utils.h"

namespace videoMosaic {

class OpenCVRenderer
{
public:
	OpenCVRenderer(const boost::property_tree::ptree& ini);

	void Process(const PolygonList& polygons, cv::Size sz, cv::Mat& output);

private:
	int m_sizeMultiplier;
	bool m_drawOutline;
};

}
