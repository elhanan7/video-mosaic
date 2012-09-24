#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

#include "image_to_mosaic.h"
#include "video_to_mhi.h"

#include <cv.h>

namespace videoMosaic {

class VideoToMosaic
{
public:
	VideoToMosaic(const boost::property_tree::ptree& ini);
	void ProcessNext(const cv::Mat_<cv::Vec3b> input, cv::Mat& output);
	void Reset();

private:
	boost::property_tree::ptree m_ini;
	boost::shared_ptr<ImageToMosaic> m_imageToMosaic;
	boost::shared_ptr<VideoToMHI> m_vtm;
	bool m_firstImage;
	double m_motionExpansionFactor;

};

}