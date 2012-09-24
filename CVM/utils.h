#pragma once

#include <cv.h>

#include <vector>

namespace videoMosaic {

	struct IdealPolygon
	{
		cv::Point center;
		float     orientation;
		cv::Vec3b color;
		cv::Size tileSize;
	};

	typedef std::vector<IdealPolygon> IdealPolygonList;

	struct Polygon
	{
		std::vector<cv::Point2d> polygon;
		IdealPolygon             ideal;
	};

	typedef std::vector<Polygon> PolygonList;

	typedef std::vector<IdealPolygon> IdealPolygonList;

	inline cv::Point clamp(cv::Point pt, cv::Size sz)
	{
		int rx = std::min(sz.width - 1, std::max(pt.x,0));
		int ry = std::min(sz.height - 1, std::max(pt.y,0));
		return cv::Point(rx, ry);
	}

}

