#pragma once

#include <cv.h>
#include <vector>

#include "clipper.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

class LocationsToPolygons
{
public:
	typedef std::vector<cv::Point2d> Polygon;
	typedef std::vector<Polygon > PolygonList;
	LocationsToPolygons(const boost::property_tree::ptree& ini);

	void Process(const std::vector<cv::Point>& locations, const std::vector<float>& os, const cv::Size& sz, PolygonList& polygons);
private:
	int m_tsize;

	ClipperLib::Polygon CreatePolygon(cv::Point pt, float o);
};

