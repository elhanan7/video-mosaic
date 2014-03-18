#include "topological_map_maker.h"

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

namespace videoMosaic {

TopologicalMapMaker::TopologicalMapMaker(const bpt::ptree&)
{
}

void TopologicalMapMaker::Process(const cv::Mat& in, cv::Mat& topo, cv::Size2f tsize, cv::Mat& distance, cv::Mat& dx, cv::Mat& dy)
{
	cv::Mat floatImg(in.rows, in.cols, CV_32FC1);
	cv::bitwise_not(in, topo);
	cv::distanceTransform(topo, floatImg, CV_DIST_L2, CV_DIST_MASK_PRECISE);
	topo.create(in.rows, in.cols, CV_8UC1);
	for (int i = 0; i < topo.rows; ++i)
	{
		float* inPtr = floatImg.ptr<float>(i);
		unsigned char* outPtr = topo.ptr<unsigned char>(i);
		for (int j = 0; j < topo.cols; ++j)
		{
			outPtr[j] = (static_cast<int>(inPtr[j] + (tsize.height / 2.0)) % cv::saturate_cast<int>(tsize.height) == 0) ? 255 : 0;
		}
	}
	cv::Sobel(floatImg, dx, CV_32FC1, 1, 0);
	cv::Sobel(floatImg, dy, CV_32FC1, 0, 1);
	distance = floatImg;
}

}

