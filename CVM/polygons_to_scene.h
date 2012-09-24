#pragma once

#include <vector>
#include <cv.h>

#include <boost/property_tree/ptree_fwd.hpp>

#include "utils.h"

namespace osg {class Node;}

namespace videoMosaic {

class PolygonsToScene
{
public:
	PolygonsToScene(const boost::property_tree::ptree& ini);
	osg::Node* Process(const PolygonList& polygons);

private:
	bool m_tesselate;
};

}