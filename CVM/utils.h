#pragma once

#include <cv.h>

#include <vector>

namespace videoMosaic {

	struct Polygon
	{
		std::vector<cv::Point2d> vertices;
		cv::Point2d center;
		float     orientation;
		cv::Vec3b color;
		cv::Size2f tileSize;
	};

	typedef std::vector<Polygon> PolygonList;

	template<typename T>
	inline cv::Point_<T> clamp(cv::Point_<T> pt, cv::Size sz)
	{
		T rx = std::min(static_cast<T>(sz.width - 1), std::max(pt.x,static_cast<T>(0)));
		T ry = std::min(static_cast<T>(sz.height - 1), std::max(pt.y,static_cast<T>(0)));
		return cv::Point_<T>(rx, ry);
	}

	namespace utils {
	inline cv::Point ToIntPoint(cv::Point2d pt)
	{
		return cv::Point(cv::saturate_cast<int>(pt.x), cv::saturate_cast<int>(pt.y));
	}
	}

}

