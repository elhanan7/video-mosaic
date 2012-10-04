#include "opencv_renderer.h"
#include "polygon_utils.h"

#include <boost/property_tree/ptree.hpp>

namespace videoMosaic {

OpenCVRenderer::OpenCVRenderer(const boost::property_tree::ptree& ini)
{
	m_sizeMultiplier = ini.get("OpenCVRenderer.SizeMultiplier", 3);
	m_drawOutline = ini.get("OpenCVRenderer.DrawOutline", true);
}

void OpenCVRenderer::Process(const PolygonList& polygons, cv::Size sz, cv::Mat& output)
{
	cv::Mat_<cv::Vec3b> result(sz * m_sizeMultiplier);
	result.setTo(cv::Vec3b(100,100,100));
	auto fixCoord = [&](cv::Point2d ptd) -> cv::Point2i
	{
		cv::Point2d mult = ptd * m_sizeMultiplier;
		return cv::Point2i(cv::saturate_cast<int>(mult.x), cv::saturate_cast<int>(mult.y));
	};

	for (auto iter = polygons.cbegin(); iter != polygons.cend(); ++iter)
	{
		std::vector<cv::Point2i> fixedVec;
		Polygon enlargedPoly = *iter;;
		utils::ScalePolygon(enlargedPoly, 1.0 / (0.9));
		std::transform(enlargedPoly.vertices.cbegin(), enlargedPoly.vertices.cend(), std::back_inserter(fixedVec), fixCoord);
		cv::Vec3b color = iter->color;

		cv::fillConvexPoly(result, fixedVec, cv::Scalar(color));
		if (m_drawOutline)
		{
			cv::polylines(result, fixedVec, true, cv::Scalar(cv::Vec3b(100,100,100)));
		}
	}

	output = result;
}

}