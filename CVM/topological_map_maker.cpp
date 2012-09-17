#include "topological_map_maker.h"

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

TopologicalMapMaker::TopologicalMapMaker(const bpt::ptree& ini)
{
		m_tsize = ini.get("ImageToMosaic.TileSize", 4);
}

void TopologicalMapMaker::Process(const cv::Mat& in, cv::Mat& dist, cv::Mat& dx, cv::Mat& dy)
{
	cv::Mat floatImg(in.rows, in.cols, CV_32FC1);
	cv::bitwise_not(in, dist);
	cv::distanceTransform(dist, floatImg, CV_DIST_L2, 5);
	dist.create(in.rows, in.cols, CV_8UC1);
	for (int i = 0; i < dist.rows; ++i)
	{
		float* inPtr = floatImg.ptr<float>(i);
		unsigned char* outPtr = dist.ptr<unsigned char>(i);
		for (int j = 0; j < dist.cols; ++j)
		{
			outPtr[j] = (static_cast<int>(inPtr[j]) % m_tsize == 0)? 255 : 0; 
		}
	}
	cv::Sobel(floatImg, dx, CV_32FC1, 1, 0);
	cv::Sobel(floatImg, dy, CV_32FC1, 0, 1);
}

