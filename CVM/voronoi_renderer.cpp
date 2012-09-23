#include "voronoi_renderer.h"

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
VoronoiRenderer::VoronoiRenderer(const boost::property_tree::ptree& ini)
{
	m_sizeMultiplier = ini.get("VoronoiRenderer.SizeMultiplier", 2);
}

void VoronoiRenderer::Process(const std::vector<cv::Point>& locations, cv::Mat_<cv::Vec3b> colorImage, cv::Mat& output)
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