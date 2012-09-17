#include <opencv2/imgproc/imgproc.hpp>
#include <highgui.h>
#include "guide_lines.h"

#include <algorithm>
#include <vector>
#include <iterator>

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;


GuideLines::GuideLines(const bpt::ptree& ini)
{
	m_glCutoff = ini.get("GuideLines.Cutoff", 0.6);
	m_morphologicalProcessing = ini.get("GuideLines.MorphologicalProcessing", true);
	m_contourSizeLimit = ini.get("GuideLines.ContourSizeLimit", 30);
	m_useStd = ini.get("GuideLines.UseSTD", true);
}

namespace
{
	void CalculateSTDMask(const cv::Mat& in, cv::Mat& out)
	{
		cv::GaussianBlur(in, out, cv::Size(23,23), 0);
		cv::absdiff(out,in, out);
		cv::threshold(out, out, 15,1,cv::THRESH_BINARY);
		cv::Mat se = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(4,4));
		cv::dilate(out, out, se, cv::Point(-1,-1),3 /*iterations*/);
		cv::erode(out, out, se, cv::Point(-1,-1), 3 /*iterations*/);
	}
}

void GuideLines::Process(const cv::Mat& in, cv::Mat& out)
{
	cv::Mat stdMask;
	if (m_useStd)
	{
		CalculateSTDMask(in, stdMask);
	}
	cv::Mat inCopy;
	cv::equalizeHist(in, inCopy);
	cv::Scalar mean, stdev;
	cv::meanStdDev(inCopy, mean, stdev);
	cv::threshold(cv::abs(inCopy - mean), inCopy, stdev[0]*m_glCutoff, 255, cv::THRESH_BINARY);
	cv::Laplacian(inCopy, out, 0);
	/*cv::namedWindow("Before");
	cv::imshow("Before", out);*/
	
	if (m_useStd)
	{
		cv::multiply(stdMask, out, out);
		/*cv::namedWindow("AfterSTD");
		cv::imshow("AfterSTD", out);*/
	}

	if (m_morphologicalProcessing)
	{
		typedef std::vector<std::vector<cv::Point> > Contours;
		Contours contours, bigContours;
		
		cv::findContours(out, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		std::copy_if(contours.begin(), contours.end(), std::back_inserter(bigContours), 
			[=](std::vector<cv::Point> vec) -> bool
			{
				return static_cast<int>(vec.size()) > this->m_contourSizeLimit;
			});
		out.setTo(0);
		cv::drawContours(out, bigContours, -1, 255);
		/*cv::namedWindow("AfterMorph");
		cv::imshow("AfterMorph", out);*/
	}

	cv::waitKey();
}
