#include "opencv_renderer.h"

#include <boost/property_tree/ptree.hpp>


OpenCVRenderer::OpenCVRenderer(const boost::property_tree::ptree& ini)
{
	m_sizeMultiplier = ini.get("OpenCVRenderer.SizeMultiplier", 3);
}

void OpenCVRenderer::Process(const cv::Mat_<cv::Vec3b>& colorImage, const PolygonList& polygons, cv::Mat& output)
{
	cv::Mat_<cv::Vec3b> result(m_sizeMultiplier*colorImage.rows, m_sizeMultiplier*colorImage.cols);
	result.setTo(cv::Vec3b(100,100,100));
	auto fixCoord = [&](cv::Point2d ptd) -> cv::Point2i
	{
		int clampedX = m_sizeMultiplier*std::min(std::max((int)ptd.x, 0), colorImage.cols);
		int clampedY = m_sizeMultiplier*std::min(std::max((int)ptd.y, 0), colorImage.rows);
		return cv::Point2i(clampedX, clampedY);
	};

	for (auto iter = polygons.cbegin(); iter != polygons.cend(); ++iter)
	{
		std::vector<cv::Point2i> fixedVec;
		std::transform(iter->cbegin(), iter->cend(), std::back_inserter(fixedVec), fixCoord);
		cv::Vec3b color = colorImage(cv::Point2i(fixedVec[0].x/m_sizeMultiplier, fixedVec[0].y/m_sizeMultiplier));

		cv::fillConvexPoly(result, fixedVec, cv::Scalar(color));
		cv::polylines(result, fixedVec, true, cv::Scalar(cv::Vec3b(100,100,100)));
	}

	output = result;
}