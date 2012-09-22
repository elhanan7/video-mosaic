#include "superpixels_renderer.h"

#include <boost/property_tree/ptree.hpp>

#include <iterator>

namespace
{
	void FillNeighbourVector(cv::Point center, cv::Size sz, std::vector<cv::Point>& result)
	{
		for (int r = -1; r <= 1; ++r)
		{
			for (int c = -1; c <= 1; ++c)
			{
				//if (r * c == 0) continue;
				cv::Point pt(center.x + c, center.y + r);
				if (pt.x >= sz.width || pt.y >= sz.height || pt.x < 0 || pt.y < 0)
				{
					continue;
				}
				result.push_back(pt);
			}
		}
	}

}
SuperpixelsRenderer::SuperpixelsRenderer(const boost::property_tree::ptree& ini)
{
	m_sizeMultiplier = ini.get("SuperPixelsRenderer.SizeMultiplier", 2);
}


//void SuperpixelsRenderer::Process(const std::vector<cv::Point>& locations, cv::Mat_<cv::Vec3b> colorImage, cv::Mat& output)
//{
//	cv::Mat_<unsigned long> one,two, a, b;
//	std::vector<cv::Vec3b> colors;
//	colors.push_back(cv::Vec3b());
//	colors.push_back(cv::Vec3b());
//	one = cv::Mat_<unsigned long>::zeros(MULTIPLIER*colorImage.rows, MULTIPLIER*colorImage.cols);
//	cv::Size sz(one.cols, one.rows);
//	two = one.clone();
//	a = one;
//	b = two;
//	unsigned long counter = 2;
//	for (auto iter = locations.cbegin(); iter != locations.cend(); ++iter)
//	{
//		cv::Point ptInRendered(MULTIPLIER * iter->x, MULTIPLIER * iter->y);
//		a(ptInRendered) = counter;
//		colors.push_back(colorImage(*iter));
//		counter++;
//	}
//	
//	int debugCounter = 40;
//	while (true)
//	{
//		bool change = false;
//		for (int r = 0; r < one.rows; ++r)
//		{
//			for (int c = 0; c < one.cols; ++c)
//			{
//				cv::Point pt(c,r);
//				unsigned long value = a(pt);
//				if (value < 2)
//				{
//					continue;
//				}
//
//				std::vector<cv::Point> neighbours;
//				FillNeighbourVector(pt, sz, neighbours);
//				for (auto nei = neighbours.cbegin(); nei != neighbours.cend(); ++nei)
//				{
//					unsigned long neiValue = a(*nei);
//					if (neiValue == 1)
//					{
//						continue;
//					}
//					else if (neiValue == 0 && b(*nei) == 0)
//					{
//						b(*nei) = value;
//						change = true;
//					}
//					else if (neiValue > value)
//					{
//						b(*nei) = 1;
//						change = true;
//					}
//				}
//			}
//		}
//		debugCounter--;
//		std::cout << debugCounter << std::endl;
//		cv::Mat_<unsigned long> temp;
//		temp = a;
//		a = b;
//		b = temp;
//		if (!change || debugCounter < 0)
//		{
//			break;
//		}
//		change = false;
//		std::cout << a(cv::Point(50,50)) << " " << a(cv::Point(51,50)) << std::endl;
//		std::cout << a(cv::Point(50,51)) << " " << a(cv::Point(51,51)) << std::endl;
//	}
//
//	cv::Mat_<cv::Vec3b> result = cv::Mat_<cv::Vec3b>::zeros(one.rows, one.cols);
//	for (int r = 0; r < one.rows; ++r)
//	{
//		for (int c = 0; c < one.cols; ++c)
//		{
//			cv::Point pt(c,r);
//			result(pt) = colors[a(pt)];
//		}
//	}
//	output = result;
//}

void SuperpixelsRenderer::Process(const std::vector<cv::Point>& locations, cv::Mat_<cv::Vec3b> colorImage, cv::Mat& output)
{
	std::vector<cv::Vec3b> colors;
	std::vector<cv::Point> multLocations;
	std::transform(locations.cbegin(), locations.cend(), std::back_inserter(colors),
		[&](const cv::Point& pt) -> cv::Vec3b
		{
			return colorImage(pt);
		}
	);

	std::transform(locations.cbegin(), locations.cend(), std::back_inserter(multLocations),
		[&](const cv::Point& pt) -> cv::Point
		{
			return cv::Point(m_sizeMultiplier * pt.x, m_sizeMultiplier * pt.y);
		}
	);

	cv::Mat_<unsigned char> src = cv::Mat_<unsigned char>::ones(m_sizeMultiplier * colorImage.rows, m_sizeMultiplier * colorImage.cols);
	cv::Mat_<float> dst(src.rows, src.cols);
	for (auto iter = multLocations.cbegin(); iter!= multLocations.cend(); ++iter)
	{
		src(*iter) = 0;
	}
	cv::Mat_<int> labels;
	cv::distanceTransform(src, dst, labels, CV_DIST_C, 3, CV_DIST_LABEL_PIXEL);

	cv::Mat_<float> laplacian;
	labels.convertTo(laplacian, laplacian.depth());
	cv::Mat_<unsigned char> laplacianMask;
	cv::Laplacian(laplacian, laplacian, laplacian.depth());
	laplacian = cv::abs(laplacian);
	cv::threshold(laplacian, laplacian, 0.01, 1, CV_THRESH_BINARY);
	laplacian.convertTo(laplacianMask, laplacianMask.type());

	std::map<int, int> lut;
	cv::Mat_<cv::Vec3b> result = cv::Mat_<cv::Vec3b>::zeros(src.rows, src.cols);

	int index = 0;
	for (auto iter = multLocations.cbegin(); iter != multLocations.cend(); ++iter)
	{
		lut.insert(std::make_pair(labels(*iter), index));
		result(*iter) = colors[index];
		index++;
	}

	for (int r = 0; r < result.rows; ++r)
	{
		for (int c = 0; c < result.cols; ++c)
		{
			cv::Point pt(c,r);
			std::map<int, int>::const_iterator mapIter = lut.find(labels(pt));
			if (mapIter != lut.end())
			{
				result(pt) = colors[mapIter->second];
			}
		}
	}
	result.setTo(cv::Vec3b(100,100,100), laplacianMask);
	output = result;
}