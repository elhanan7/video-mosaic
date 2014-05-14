#pragma once
#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

namespace videoMosaic
{

class GuideLines
{
 public:
   GuideLines(const boost::property_tree::ptree& ini);
   void Process(const cv::Mat& in, cv::Mat& out);

 private:
   double m_guideLinesCutoff;
   bool m_morphologicalProcessing;
   int m_contourSizeLimit;
   bool m_useStd;
};
}
