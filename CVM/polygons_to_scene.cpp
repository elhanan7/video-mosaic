#include "polygons_to_scene.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgUtil/Tessellator>
#include <osgUtil/TriStripVisitor>

#include <iterator>

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

PolygonsToScene::PolygonsToScene(const bpt::ptree& ini)
{
	m_tesselate = ini.get("OSGRenderer.Tesselate", false);
}

osg::Node* PolygonsToScene::Process(const cv::Mat_<cv::Vec3b>& img, const PolygonList& polygons)
{
	auto geom = new osg::Geometry;
	auto vertices = new osg::Vec3Array;
	auto colors = new osg::Vec4Array;
	std::vector<size_t> idxs;

	for (size_t i = 0; i < polygons.size(); ++i)
	{
		size_t n = polygons[i].size();
		cv::Vec3b color = img(cv::Point((int)polygons[i][0].x, std::max((int)polygons[i][0].y, 0)));
		osg::Vec4 clr = osg::Vec4(color[2] / 155.0f, color[1] / 155.0f, color[0] / 155.0f, 1.0f);
		for (size_t j = 0; j < n; ++j) colors->push_back(clr);
		std::transform(polygons[i].begin(), polygons[i].end(), std::back_inserter(*vertices), 
			[](const cv::Point2d& pt) -> osg::Vec3
		{
			return osg::Vec3(pt.x, pt.y, 0);
		});
		idxs.push_back(n);
	}

	geom->setVertexArray(vertices);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	auto normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0,0,-1));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	size_t currIdx = 0;
	for (size_t i = 0; i < idxs.size(); ++i)
	{
		geom->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON, currIdx, idxs[i]));
		currIdx += idxs[i];
	}

	if (m_tesselate)
	{
		osg::ref_ptr<osgUtil::Tessellator> tess = new osgUtil::Tessellator;
		tess->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
		tess->setBoundaryOnly(false);
		tess->setWindingType( osgUtil::Tessellator::TESS_WINDING_NONZERO);
		osg::ref_ptr<osgUtil::TriStripVisitor> trivi = new osgUtil::TriStripVisitor;
		tess->retessellatePolygons(*geom);
	}
	
	osg::Geode* gd = new osg::Geode;
	gd->addDrawable(geom);
	return gd;
}