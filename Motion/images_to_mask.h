#pragma once

#include <cv.h>

namespace videoMosaic
{

class ImagesToMask
{
 public:
   ImagesToMask(void);
   ~ImagesToMask(void);

   void Process(const cv::Mat& a, const cv::Mat b, cv::Point2f globalTrans,
                cv::Mat& mask);
};
}
