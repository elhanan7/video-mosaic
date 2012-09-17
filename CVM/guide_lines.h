#pragma once
#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

class GuideLines
{
public:
	GuideLines(const boost::property_tree::ptree& ini);
	void Process(const cv::Mat& in, cv::Mat& out);

private:
	double m_glCutoff;
	bool m_morphologicalProcessing;
	int m_contourSizeLimit;
	bool m_useStd;
};

