#include "video_to_mhi.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/circular_buffer.hpp>
#include <vector>

namespace bpt = boost::property_tree;

namespace videoMosaic {

class VideoToMHIImpl
{
public:
	VideoToMHIImpl(const bpt::ptree& ini);

	void Give(const cv::Mat input);
	cv::Mat_<float> Take();
	void TakeSegmentation(std::vector<cv::Rect>& segmentation);

private:
	unsigned int m_bufferLength;
	unsigned int m_frameRate;
	int m_threshold;
	cv::Mat_<float> m_mhi;
	typedef boost::circular_buffer<cv::Mat_<float> > CircularBuffer;
	CircularBuffer m_buffer;
	unsigned int m_currentFrame;
	unsigned int m_minimalComponentDimension;
};


VideoToMHIImpl::VideoToMHIImpl(const bpt::ptree& ini)
{
	m_bufferLength =  ini.get("VideoToMHI.BufferLength", 10);
	m_threshold =     ini.get("VideoToMHI.MotionThreshold", 50);
	m_frameRate =     ini.get("VideoToMHI.FrameRate", 10);
	m_minimalComponentDimension = ini.get("VideoToMHI.MinimalComponentDimension", 25);
	m_currentFrame = 0;
	m_buffer = CircularBuffer(m_bufferLength);
}

void VideoToMHIImpl::Give(const cv::Mat input)
{
	if (m_buffer.size() == 0)
	{
		m_mhi = cv::Mat_<float>::zeros(input.rows, input.cols);
	}
	cv::Mat_<unsigned char> mat, diff;
	input.convertTo(mat, mat.type());
	m_buffer.push_back(mat);
	cv::Mat b, e;
	b = m_buffer.front();
	e = m_buffer.back();
	cv::absdiff(e, b, diff);
	cv::threshold(diff, diff, m_threshold, 1,CV_THRESH_BINARY);
	cv::updateMotionHistory(diff, m_mhi,m_currentFrame, m_frameRate);
	m_currentFrame = (m_currentFrame + 1);
}

cv::Mat_<float> VideoToMHIImpl::Take()
{
	return m_mhi;
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

void VideoToMHI::Give(const cv::Mat input)
{
	m_pimpl->Give(input);
}

cv::Mat_<float> VideoToMHI::Take()
{
	return m_pimpl->Take();
}

void VideoToMHI::TakeSegmentation(std::vector<cv::Rect>& segmentation)
{
	m_pimpl->TakeSegmentation(segmentation);
}

}