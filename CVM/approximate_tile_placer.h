#pragma once

#include "polygon_utils.h"

namespace videoMosaic
{
	struct ApproximatePlacerTraits
	{
		ApproximatePlacerTraits(cv::Size imsz, cv::Mat_<unsigned char> tileMask, cv::Size2f tsz, cv::Mat dxx, cv::Mat dyy) : 
			imSize(imsz), tSize(tsz), mask(tileMask) , dx(dxx), dy(dyy)
		{
			half = cv::Point(cv::saturate_cast<int>(tSize.width - 1), cv::saturate_cast<int>(tSize.height - 1));
			ones = cv::Point(1,1);
		};

		typedef Polygon PolygonType;

		PolygonType GetPolygon(cv::Point pt)
		{
			float orientation = utils::GetOrientation(dx(pt), dy(pt));
			return utils::CreateSimplePolygon(pt, orientation, tSize);
		}

		void SetTileSize(cv::Size2f tsz)  {tSize = tsz;}

		void CheckUpdate(const Polygon& poly, PolygonList& polygons)
		{
			cv::Point pt = poly.center;
			cv::Point tl = pt - half + ones;
			cv::Point br = pt + half;
			tl = clamp(tl, imSize);
			br = clamp(br, imSize);
			if (cv::sum(mask(cv::Rect(tl, br)))[0] > 0) 
			{
				return;
			}

			mask(pt) = 255;
			polygons.push_back(poly);
		}

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
		cv::Mat_<unsigned char> mask;
		cv::Point half, ones;
		cv::Mat_<float> dx, dy;
	};

}