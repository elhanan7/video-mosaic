#include "locations_to_polygons.h"
#include "locator_grid.h"
#include "transfromations.h"

#include "clipper.hpp"
#include <cv.h>

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

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
}

LocationsToPolygons::LocationsToPolygons(const bpt::ptree& ini)
{
	m_tsize = ini.get("ImageToMosaic.TileSize", 4);
}

void LocationsToPolygons::Process(const std::vector<cv::Point>& locations, const std::vector<float>& os , const cv::Size& sz, PolygonList& polygons)
{
	LocatorGrid grid(sz, m_tsize);
	for (size_t i = 0; i < locations.size(); ++i)
	{
		grid.Add(locations[i], i);
	}

	ClipperLib::Polygons originalPolys(locations.size());
	ClipperLib::ExPolygons clipperResult;
	polygons.clear();
	polygons.reserve(static_cast<size_t>(originalPolys.size()*1.5));

	for (size_t i = 0; i < locations.size(); ++i)
	{
		originalPolys[i] = CreatePolygon(locations[i], os[i]);
	} 

	std::vector<size_t> hood;
	
	for (size_t i = 0; i < locations.size(); ++i)
	{
		ClipperLib::ExPolygons result;
		ClipperLib::Clipper clipper;
		clipper.AddPolygon(originalPolys[i], ClipperLib::ptSubject);

		grid.GetHood(locations[i], hood);
		for (size_t j = 0; j < hood.size(); ++j)
		{
			if (hood[j] < i)
			{
				clipper.AddPolygon(originalPolys[hood[j]], ClipperLib::ptClip);
			}
		}
		
		clipper.Execute(ClipperLib::ctDifference, result);
		clipperResult.insert(clipperResult.end(), result.begin(), result.end());
	}
	polygons.resize(clipperResult.size());
	for (size_t i = 0; i < clipperResult.size(); ++i)
	{
		std::transform(clipperResult[i].outer.begin(), clipperResult[i].outer.end(), 
			std::back_inserter(polygons[i]), &ToCv);
	}

	
}

ClipperLib::Polygon LocationsToPolygons::CreatePolygon(cv::Point pt, float o)
{
	ClipperLib::Polygon poly;
	std::vector<cv::Point2d> temp;
	double half = m_tsize / 3.0;
	temp.push_back(cv::Point2d(-half, -half));
	temp.push_back(cv::Point2d( half, -half));
	temp.push_back(cv::Point2d( half,  half));
	temp.push_back(cv::Point2d(-half,  half));
	transformations::Shift shift(pt.y ,pt.x);
	transformations::Rotate rot(o);
	std::transform(temp.begin(), temp.end(), temp.begin(), shift*rot);
	std::transform(temp.begin(), temp.end(), std::back_inserter(poly), &FromCvD);
	return poly;
}