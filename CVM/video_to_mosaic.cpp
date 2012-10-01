#include "video_to_mosaic.h"

#include <boost/property_tree/ptree_fwd.hpp>

namespace videoMosaic {

VideoToMosaic::VideoToMosaic(const boost::property_tree::ptree& ini)
{
	m_ini = ini;
	Reset();
}

void VideoToMosaic::Reset()
{
	m_imageToMosaic = boost::shared_ptr<ImageToMosaic>(new ImageToMosaic(m_ini));
	m_vtm = boost::shared_ptr<VideoToMHI>(new VideoToMHI(m_ini));
	m_followMotionStrictly = m_ini.get("VideoToMosaic.FollowMotionStrictly", true);
	m_motionExpansionFactor = m_ini.get("VideoToMosaic.MotionExpansionFactor", 1.5);
	m_firstImage = true;
}

void VideoToMosaic::ProcessNext(const cv::Mat_<cv::Vec3b> input, cv::Mat& output)
{
	cv::Mat gray;
	cv::Mat_<cv::Vec3b> result(input.size());
	cv::cvtColor(input, gray, CV_BGR2GRAY);
	m_vtm->Give(gray);
	cv::Mat_<float> mhi = m_vtm->Take().clone();
	std::vector<cv::Rect> segs;
	m_vtm->TakeSegmentation(segs);
	cv::Mat_<unsigned char> motionMask = cv::Mat::zeros(input.rows, input.cols, CV_8U);
	for (auto iter = segs.cbegin(); iter != segs.cend(); ++iter)
	{
		cv::Rect rect = *iter;
		cv::Size axes(m_motionExpansionFactor*rect.width, m_motionExpansionFactor*rect.height);
		cv::Point center(rect.x + 0.5*rect.width, rect.y + 0.5*rect.height);
		if (m_followMotionStrictly)
		{
			cv::Mat mhiROI, motionMaskROI;
			mhiROI = mhi(rect);
			motionMaskROI = motionMask(rect);
			cv::Mat currentMotion, currentMotionUC;
			cv::threshold(mhiROI, currentMotion, m_vtm->TakeTimeStamp() - 10, 1, cv::THRESH_BINARY);
			currentMotion.convertTo(currentMotionUC, CV_8U);
			cv::dilate(currentMotionUC, currentMotionUC, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(10,10)));
			currentMotionUC.copyTo(motionMaskROI);
		}
		else
		{
			cv::ellipse(motionMask, center, axes,0 ,0, 360,cv::Scalar(1),-1);
		}
	}
	if (!m_firstImage)
	{
		m_imageToMosaic->Process(input, result, motionMask);
	}
	else
	{
		m_imageToMosaic->Process(input, result);
		m_firstImage = false;
	}
	output = result;
}

}
