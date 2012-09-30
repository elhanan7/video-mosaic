#pragma once
#include "config.h"

#ifdef USE_OSG
#include <osg/Node>
#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

namespace videoMosaic {

class SceneToImage
{
public:
	SceneToImage(const boost::property_tree::ptree& ini);

	void Process(osg::Node*, cv::Size sz, cv::Mat& res);
};

}
#endif
