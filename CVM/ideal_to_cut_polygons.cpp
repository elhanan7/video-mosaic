#include "ideal_to_cut_polygons.h"
#include "locator_grid.h"
#include "transfromations.h"

#include "clipper.hpp"
#include <cv.h>

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

namespace videoMosaic {

namespace
{
	cv::Point2d ToCv(const ClipperLib::IntPoint& pt)
	{
		return cv::Point2d((double)pt.Y / ((ClipperLib::long64)1 << 50), (double)pt.X / ((ClipperLib::long64)1 << 50));
	}

	ClipperLib::IntPoint FromCvD(const cv::Point2d& pt)
	{
		return ClipperLib::IntPoint(static_cast<ClipperLib::long64>(pt.x * ((ClipperLib::long64)1 << 50)),
										static_cast<ClipperLib::long64>(pt.y * ((ClipperLib::long64)1 << 50)));
	}

	ClipperLib::Polygon CreatePolygon(const Polygon& ideal)
	{
		ClipperLib::Polygon poly;
		std::vector<cv::Point2d> temp;
		double halfx = ideal.tileSize.width / 3.0;
		double halfy = ideal.tileSize.height / 3.0;
		temp.push_back(cv::Point2d(-halfx, -halfy));
		temp.push_back(cv::Point2d( halfx, -halfy));
		temp.push_back(cv::Point2d( halfx,  halfy));
		temp.push_back(cv::Point2d(-halfx,  halfy));
		transformations::Shift shift(ideal.center.y ,ideal.center.x);
		transformations::Rotate rot(ideal.orientation);
		std::transform(temp.begin(), temp.end(), temp.begin(), shift*rot);
		std::transform(temp.begin(), temp.end(), std::back_inserter(poly), &FromCvD);
		return poly;
	}
}

IdealToCutPolygon::IdealToCutPolygon(const bpt::ptree&)
{
}

void IdealToCutPolygon::Process(const PolygonList& ideal, const cv::Size2f& tsize, const cv::Size& imsize, PolygonList& polygons)
{
	LocatorGrid grid(imsize, static_cast<int>(std::max(tsize.width, tsize.height)));
	for (size_t i = 0; i < ideal.size(); ++i)
	{
		grid.Add(ideal[i].center, i);
	}

	ClipperLib::Polygons originalPolys(ideal.size());
	ClipperLib::ExPolygons clipperResult;
	std::vector<size_t> idealIndexes;
	polygons.clear();
	polygons.reserve(static_cast<size_t>(originalPolys.size()*1.5));

	for (size_t i = 0; i < ideal.size(); ++i)
	{
		originalPolys[i] = CreatePolygon(ideal[i]);
	} 

	std::vector<size_t> hood;
	
	for (size_t i = 0; i < ideal.size(); ++i)
	{
		ClipperLib::ExPolygons result;
		ClipperLib::Clipper clipper;
		clipper.AddPolygon(originalPolys[i], ClipperLib::ptSubject);

		grid.GetHood(ideal[i].center, hood);
		for (size_t j = 0; j < hood.size(); ++j)
		{
			if (hood[j] < i)
			{
				clipper.AddPolygon(originalPolys[hood[j]], ClipperLib::ptClip);
			}
		}
		
		clipper.Execute(ClipperLib::ctDifference, result);
		clipperResult.insert(clipperResult.end(), result.begin(), result.end());
		for (size_t k = 0; k < result.size(); ++k)
		{
			idealIndexes.push_back(i);
		}
	}
	polygons.resize(clipperResult.size());
	for (size_t i = 0; i < clipperResult.size(); ++i)
	{
		polygons[i] = ideal[idealIndexes[i]];
		polygons[i].vertices.clear();
		std::transform(clipperResult[i].outer.begin(), clipperResult[i].outer.end(), 
			std::back_inserter(polygons[i].vertices), &ToCv);
	}

	
}

}