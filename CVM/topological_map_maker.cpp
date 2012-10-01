#include "topological_map_maker.h"

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

namespace videoMosaic {

TopologicalMapMaker::TopologicalMapMaker(const bpt::ptree&)
{
}

void TopologicalMapMaker::Process(const cv::Mat& in, cv::Mat& dist, cv::Size2f tsize, cv::Mat& dx, cv::Mat& dy)
{
	cv::Mat floatImg(in.rows, in.cols, CV_32FC1);
	cv::bitwise_not(in, dist);
	cv::distanceTransform(dist, floatImg, CV_DIST_L2, CV_DIST_MASK_PRECISE);
	dist.create(in.rows, in.cols, CV_8UC1);
	for (int i = 0; i < dist.rows; ++i)
	{
		float* inPtr = floatImg.ptr<float>(i);
		unsigned char* outPtr = dist.ptr<unsigned char>(i);
		for (int j = 0; j < dist.cols; ++j)
		{
			outPtr[j] = (static_cast<int>(inPtr[j]) % cv::saturate_cast<int>(tsize.height) == 0)? 255 : 0; 
		}
	}
	cv::Sobel(floatImg, dx, CV_32FC1, 1, 0);
	cv::Sobel(floatImg, dy, CV_32FC1, 0, 1);
}

}

