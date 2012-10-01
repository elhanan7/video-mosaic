#pragma once
#include <math.h>

#include "utils.h"
#include "transfromations.h"

namespace videoMosaic
{
	namespace utils 
	{
		
		float GetOrientation(float dx, float dy)
		{
			return atan2(dy, dx);
		}

		Polygon CreateSimplePolygon(cv::Point2d center, float orientation, cv::Size2f tsize)
		{
			Polygon poly;
			poly.center = center;
			poly.orientation = orientation;
			poly.tileSize = tsize;

			double halfx = poly.tileSize.width / 2.0;
			double halfy = poly.tileSize.height / 2.0;
			poly.vertices.push_back(cv::Point2d(-halfx, -halfy));
			poly.vertices.push_back(cv::Point2d( halfx, -halfy));
			poly.vertices.push_back(cv::Point2d( halfx,  halfy));
			poly.vertices.push_back(cv::Point2d(-halfx,  halfy));
			transformations::Shift shift(poly.center.x ,poly.center.y);
			transformations::Rotate rot(poly.orientation);
			std::transform(poly.vertices.begin(), poly.vertices.end(), poly.vertices.begin(), shift*rot);
			return poly;
		}



	}
}