#pragma once

#include <vector>
#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

namespace osg {class Node;}

class PolygonsToScene
{
public:
	PolygonsToScene(const boost::property_tree::ptree& ini);
	typedef std::vector<cv::Point2d> Polygon;
	typedef std::vector<Polygon> PolygonList;
	osg::Node* Process(const cv::Mat_<cv::Vec3b>& img, const PolygonList& polygons);

private:
	bool m_tesselate;
};

