#pragma once

#include <cv.h>

namespace videoMosaic {

class VideoSource
{
public:
	virtual cv::Mat Next() = 0;
	virtual bool HasNext() const = 0;
	virtual void Reset();
	virtual cv::Size Size() const  = 0;
	virtual int Type() const = 0;
	virtual ~VideoSource(void) {};
};

inline void VideoSource::Reset() { throw new std::logic_error("Not implemented"); }
}

