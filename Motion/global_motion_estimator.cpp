#include "global_motion_estimator.h"

#include <boost/property_tree/ptree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

namespace videoMosaic {
GlobalMotionEstimator::GlobalMotionEstimator(const boost::property_tree::ptree& ini)
{
	m_distanceThreshold = ini.get("GlobalMotionEstimator.DistanceThreshold", 25.0f);
	m_minimumRequiredMatches = ini.get("GlobalMotionEstimator.MinimumRequiredMatches", 10);
}

bool GlobalMotionEstimator::Estimate(cv::Mat_<unsigned char> from, cv::Mat_<unsigned char> to , cv::Mat& trans)
{
	static cv::Ptr<cv::FeatureDetector> siftDetect = 
		cv::Ptr<cv::FeatureDetector>(new cv::FastFeatureDetector());
	static cv::Ptr<cv::DescriptorExtractor> siftExtract = 
		cv::Ptr<cv::DescriptorExtractor>(new cv::SIFT());
	static cv::Ptr<cv::DescriptorMatcher> flann = cv::DescriptorMatcher::create("FlannBased");

	trans = cv::Mat_<double>::eye(cv::Size(3,3));

	std::vector<cv::KeyPoint> kps1, kps2;
	siftDetect->detect(from, kps1);
	siftDetect->detect(to, kps2);

	cv::Mat descs1, descs2;
	siftExtract->compute(from, kps1, descs1);
	siftExtract->compute(to, kps2, descs2);

	if (kps1.size() < 10 || kps2.size() < 10)
	{
		return false;
	}
	
	std::vector<cv::DMatch> matches, goodMatches;
	flann->match(descs1, descs2, matches);

	std::copy_if(matches.begin(), matches.end(), std::back_inserter(goodMatches), [&](const cv::DMatch& match) -> bool
	{
		return match.distance < m_distanceThreshold;
	});

	if (goodMatches.size() < m_minimumRequiredMatches)
	{
		return false;
	}

	std::vector<cv::Point2d> pixes1, pixes2;

	for (auto iter = goodMatches.cbegin(); iter != goodMatches.cend(); ++iter)
	{
		pixes1.push_back(kps1[iter->queryIdx].pt);
		pixes2.push_back(kps2[iter->trainIdx].pt);
	}

	trans = cv::findHomography(pixes1, pixes2, cv::RANSAC, 2);
	return true;
}

void GlobalMotionEstimator::CalculateValidMask(cv::Mat trans, const cv::Size& sz, cv::Mat& mask)
{
	cv::Mat zer = cv::Mat_<unsigned char>::zeros(sz); 
	cv::warpPerspective(zer, mask, trans, sz, 1, cv::BORDER_CONSTANT, 1);
}

}
