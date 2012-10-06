#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/shared_ptr.hpp>

#include <cv.h>

namespace videoMosaic {

class VideoToMHIImpl;

class VideoToMHI
{
public:
	VideoToMHI(const boost::property_tree::ptree& ini);

	void Give(const cv::Mat input, bool globalMotion = false);
	cv::Mat_<float> Take();
	bool TakeGlobalTrans(cv::Mat& trans);
	void TakeSegmentation(std::vector<cv::Rect>& segmentation);
	double TakeTimeStamp();

private:
	boost::shared_ptr<VideoToMHIImpl> m_pimpl;

};

}
