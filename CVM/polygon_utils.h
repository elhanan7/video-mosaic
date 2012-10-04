#pragma once
#include <math.h>

#include "utils.h"
#include "transfromations.h"

namespace videoMosaic
{
	namespace utils 
	{
		
		inline float GetOrientation(float dx, float dy)
		{
			return atan2(dy, dx);
		}

		inline Polygon CreateSimplePolygon(cv::Point2d center, float orientation, cv::Size2f tsize)
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

		inline cv::Point2d FindCenter(const Polygon& poly)
		{
			cv::Point2d sum(0,0);
			for (auto iter = poly.vertices.cbegin(); iter != poly.vertices.cend(); ++iter)
			{
				sum += *iter;
			}
			sum = sum * (1/static_cast<double>(poly.vertices.size()));
			return sum;
		}

		inline void ScalePolygon(Polygon& poly, double factor)
		{
			cv::Point2d center = FindCenter(poly);
			transformations::Shift bringToOrigin(-center.x, -center.y);
			transformations::Scale scale(factor);
			transformations::Shift backToPlace(center.x, center.y);
			std::transform(poly.vertices.begin(), poly.vertices.end(), poly.vertices.begin(), backToPlace*scale*bringToOrigin);
		}



	}
}