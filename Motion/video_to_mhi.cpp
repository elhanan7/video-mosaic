#include "video_to_mhi.h"
#include "global_motion_estimator.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/circular_buffer.hpp>
#include <vector>

namespace bpt = boost::property_tree;

namespace videoMosaic {

namespace
{
	void StabilizeTransform(cv::Mat_<double>& trans)
	{
		static const double EPS = 0.1; 
		if (std::abs(trans(cv::Point(0,0))) - 1 < EPS ) trans(cv::Point(0,0)) = 1;
		if (std::abs(trans(cv::Point(1,1))) - 1 < EPS ) trans(cv::Point(1,1)) = 1;
		if (std::abs(trans(cv::Point(0,1))) < EPS ) trans(cv::Point(0,1)) = 0;
		if (std::abs(trans(cv::Point(1,0))) < EPS ) trans(cv::Point(1,0)) = 0;
		if (std::abs(trans(cv::Point(2,0))) < EPS ) trans(cv::Point(2,0)) = 0;
		if (std::abs(trans(cv::Point(2,1))) < EPS ) trans(cv::Point(2,1)) = 0;
		if (std::abs(trans(cv::Point(0,2))) < EPS ) trans(cv::Point(0,2)) = 0;
		if (std::abs(trans(cv::Point(1,2))) < EPS ) trans(cv::Point(1,2)) = 0;
		if (std::abs(trans(cv::Point(2,2))) - 1 < EPS ) trans(cv::Point(2,2)) = 1;
	}
}
class VideoToMHIImpl
{
public:
	VideoToMHIImpl(const bpt::ptree& ini);

	void Give(const cv::Mat input, bool globalMotion);
	cv::Mat_<float> Take();
	bool TakeGlobalTrans(cv::Mat& trans);
	void TakeSegmentation(std::vector<cv::Rect>& segmentation);
	double TakeTimeStamp() { return m_currentFrame - 1;}

private:
	GlobalMotionEstimator m_motionEstimator;
	unsigned int m_bufferLength;
	unsigned int m_frameRate;
	int m_threshold;
	cv::Mat_<float> m_mhi;
	bool m_transValid;
	cv::Mat_<double> m_globalTrans;
	typedef boost::circular_buffer<cv::Mat_<unsigned char> > CircularBuffer;
	CircularBuffer m_buffer;
	unsigned int m_currentFrame;
	unsigned int m_minimalComponentDimension;
};


VideoToMHIImpl::VideoToMHIImpl(const bpt::ptree& ini) : m_motionEstimator(ini)
{
	m_bufferLength =  ini.get("VideoToMHI.BufferLength", 10);
	m_threshold =     ini.get("VideoToMHI.MotionThreshold", 50);
	m_frameRate =     ini.get("VideoToMHI.FrameRate", 10);
	m_minimalComponentDimension = ini.get("VideoToMHI.MinimalComponentDimension", 25);
	m_currentFrame = 0;
	m_buffer = CircularBuffer(m_bufferLength);
	m_transValid = false;
	m_globalTrans = cv::Mat_<double>::eye(cv::Size(3,3));
}

void VideoToMHIImpl::Give(const cv::Mat input, bool globalMotion)
{
	if (m_buffer.size() == 0)
	{
		m_mhi = cv::Mat_<float>::zeros(input.rows, input.cols);
	}
	cv::Mat_<unsigned char> mat;
	input.convertTo(mat, mat.type());
	m_buffer.push_back(mat);
	cv::Mat_<unsigned char> b, e, tmp, warpMask;
	cv::Mat_<float> bf, ef, diff;
	cv::Mat_<double> distantMotionTrans;
	b = m_buffer.front();
	e = m_buffer.back();
	if (globalMotion)
	{
		m_motionEstimator.Estimate(m_buffer.front(), m_buffer.back(), distantMotionTrans);
		StabilizeTransform(distantMotionTrans);
		m_globalTrans = cv::Mat_<double>::eye(cv::Size(3,3));
		if (m_buffer.size() > 1)
		{
			m_transValid = m_motionEstimator.Estimate(*(m_buffer.rbegin() + 1), *m_buffer.rbegin(), m_globalTrans);
			StabilizeTransform(m_globalTrans);
		}
		if (!m_transValid)
		{
			m_buffer.clear();
			m_buffer.push_back(e);
		}
		cv::warpPerspective(b, tmp, distantMotionTrans, b.size(), 1, cv::BORDER_DEFAULT);
		b = tmp;
		m_motionEstimator.CalculateValidMask(distantMotionTrans, m_mhi.size(), warpMask);
		cv::warpPerspective(m_mhi, diff, m_globalTrans, m_mhi.size(), cv::INTER_NEAREST);
		m_mhi = diff;
	}

	b.convertTo(bf, bf.type());
	e.convertTo(ef, ef.type());
	cv::absdiff(ef, bf, diff);
	cv::threshold(diff, diff, m_threshold, 1,CV_THRESH_BINARY);
	if (globalMotion)
	{
		diff.setTo(1, warpMask);
	}
	diff.convertTo(warpMask, warpMask.type());
	cv::updateMotionHistory(warpMask, m_mhi,m_currentFrame, m_frameRate);
	m_currentFrame = (m_currentFrame + 1);
}

cv::Mat_<float> VideoToMHIImpl::Take()
{
	return m_mhi;
}

bool VideoToMHIImpl::TakeGlobalTrans(cv::Mat& trans)
{
	trans = m_globalTrans;
	return m_transValid;
}

void VideoToMHIImpl::TakeSegmentation(std::vector<cv::Rect>& segmentation)
{
	std::vector<cv::Rect> segs;
	cv::Mat segmask;
	cv::segmentMotion(m_mhi, segmask, segs, m_currentFrame - 1, 1);
	std::copy_if(segs.cbegin(), segs.cend(), std::back_inserter(segmentation), 
		[&] (const cv::Rect& rect) -> bool
		{
			return (unsigned int) rect.height > m_minimalComponentDimension &&
				(unsigned int) rect.width > m_minimalComponentDimension;
		}
	);
}



VideoToMHI::VideoToMHI(const bpt::ptree& ini)
{
	m_pimpl = boost::shared_ptr<VideoToMHIImpl>(new VideoToMHIImpl(ini));
}

void VideoToMHI::Give(const cv::Mat input, bool globalMotion)
{
	m_pimpl->Give(input, globalMotion);
}

cv::Mat_<float> VideoToMHI::Take()
{
	return m_pimpl->Take();
}

void VideoToMHI::TakeSegmentation(std::vector<cv::Rect>& segmentation)
{
	m_pimpl->TakeSegmentation(segmentation);
}

double VideoToMHI::TakeTimeStamp()
{
	return m_pimpl->TakeTimeStamp();
}

bool VideoToMHI::TakeGlobalTrans(cv::Mat& trans)
{
	return m_pimpl->TakeGlobalTrans(trans);
}

}