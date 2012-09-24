#pragma once

#include <cv.h>
#include <boost/property_tree/ptree_fwd.hpp>

#include "utils.h"

namespace videoMosaic {

class PovRayRenderer
{
public:
	PovRayRenderer(const boost::property_tree::ptree& ini);

	void Process(const PolygonList& polys, cv::Size sz, cv::Mat& res);

private:
	int m_sizeMultiplier;
	std::string m_povrayPath;
	std::string m_povIniPath;
	float m_tileHeight;
};

}