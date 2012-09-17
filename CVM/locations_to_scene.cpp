#include "locations_to_scene.h"

#include <osg/Node>
#include <osg/Matrix>
#include <osg/Geode>
#include <osg/Geometry>

namespace {
	
}

osg::Node* LocationsToScene::Process(const cv::Mat_<cv::Vec3b>& clrImage, const PointList& centers, const OrientationList& os)
{
	auto geom = new osg::Geometry;
	auto vertices = new osg::Vec3Array;
	auto colors = new osg::Vec4Array;

	for (size_t i = 0; i < centers.size(); ++i)
	{
		auto cntr = osg::Vec3(centers[i].x, centers[i].y, 0);
		float half = m_tsize / 2;
		osg::Vec3 bl, br, tr, tl;
		osg::Matrix mat = osg::Matrix::rotate(os[i], osg::Vec3(0,0,1));
		bl = cntr + mat * osg::Vec3(-half,-half,0);
		br = cntr + mat * osg::Vec3( half,-half,0);
		tr = cntr + mat * osg::Vec3( half, half,0);
		tl = cntr + mat * osg::Vec3(-half, half,0);
		cv::Vec3b color = clrImage(centers[i]);
		osg::Vec4 clr = osg::Vec4(color[2] / 255.0f, color[1] / 255.0f, color[0] / 255.0f, 1.0f);
		for (int j = 0; j < 4; ++j) colors->push_back(clr);
		vertices->push_back(bl);
		vertices->push_back(br);
		vertices->push_back(tr);
		vertices->push_back(tl);
	}

	geom->setVertexArray(vertices);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	auto normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0,0,-1));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4*centers.size()));
	osg::Geode* gd = new osg::Geode;
	gd->addDrawable(geom);
	return gd;
}
