#pragma once

#include "utils.h"

namespace videoMosaic
{
	struct PrecisePlacerTraits
	{
		PrecisePlacerTraits(cv::Mat_<float> dxx, cv::Mat_<float> dyy, cv::Size2f tsz, float maxAreaPercent) : imSize(dxx.size()), tSize(tsz), dx(dxx), dy(dyy),
			maxPercent(maxAreaPercent)
		{
			mask = cv::Mat_<unsigned char>::zeros(imSize);
			half = cv::Point2f(tSize.width - 1, tSize.height - 1);
			ones = cv::Point2f(1,1);
		};

		typedef Polygon PolygonType;

		PolygonType GetPolygon(cv::Point pt)
		{
			Polygon poly;
			poly.center = pt;
			poly.orientation = atan2(dy(pt), dx(pt));
			poly.tileSize = tSize;

			std::vector<cv::Point2d> temp;
			double halfx = poly.tileSize.width / 2;
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

		bool CheckUpdate(const PolygonType& poly)
		{
			singlePolyMask.create(imSize);
			singlePolyMask.setTo(0);
			std::vector<cv::Point2i> intVec;
			std::transform(poly.vertices.cbegin(), poly.vertices.cend(), std::back_inserter(intVec), [&](const cv::Point2d& dpt) -> cv::Point
			{
				cv::Point intPt(cv::saturate_cast<int>(dpt.x), cv::saturate_cast<int>(dpt.y));
				return clamp(intPt, this->imSize);
			});
			cv::fillConvexPoly(singlePolyMask, intVec, cv::Scalar(255));
			cv::Rect roi(poly.center - cv::Point2d(tSize.width, tSize.height), tSize * 2.0f);
			cv::Rect fullRoi(cv::Point(0,0), imSize);
			roi = roi & fullRoi;
			cv::Mat_<unsigned char> maskRoi, singlePolyRoi, roiResult;
			maskRoi = mask(roi);
			singlePolyRoi = singlePolyMask(roi);
			cv::bitwise_and(maskRoi, singlePolyRoi, roiResult);
			int n = cv::saturate_cast<int>(cv::sum(roiResult)[0] / 255);
			float percent = (float) n / roi.area();

			if (percent > maxPercent)
			{
				return false;
			}
			cv::bitwise_or(maskRoi, singlePolyRoi, maskRoi);
			return true;
		}

		void SetTileSize(cv::Size2f tsz)  {tSize = tsz;}

		cv::Mat_<unsigned char> GetMask()
		{
			return mask;
		}

		static bool NeedsClipping()
		{
			return true;
		}

		cv::Size imSize;
	    cv::Size2f tSize;
		cv::Mat_<float> dx,dy;
		float maxPercent;
		cv::Mat_<unsigned char> mask, singlePolyMask;
		cv::Point2f half, ones;
	};
}