#include "config.h"

#ifdef USE_OSG

#include <Windows.h>
#include "scene_to_image.h"
#include <osgViewer/Viewer>
#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

void osr(osg::Node* scene, osgViewer::Viewer* viewer, unsigned int width, unsigned int height, cv::Mat* cvmat);

namespace videoMosaic {

SceneToImage::SceneToImage(const bpt::ptree&)
{
}


void SceneToImage::Process(osg::Node* scene, cv::Size sz, cv::Mat& res)
{
	osgViewer::Viewer viewer;
	viewer.setCameraManipulator(NULL);
	//viewer.getCamera()->setGraphicsContext(GetOffscreenGraphicsContext(sz.width, sz.height));
	viewer.getCamera()->setViewport(0,0,sz.width, sz.height);
	viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3(0, 0, sz.width/2.0f),
		                                      osg::Vec3(0, 0, 0),
											  osg::Vec3(0, 1, 0));
	viewer.getCamera()->setProjectionMatrixAsOrtho2D(0, sz.width, sz.height, 0);
	osg::Light* light = new osg::Light(0);
	light->setPosition(osg::Vec4(0, 0, -sz.width/2.0f,0));
	//osg::Image* img = new osg::Image;
	//img->allocateImage(sz.width, sz.height, 1, GL_RGB, GL_UNSIGNED_BYTE);
	//osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    //camera->setClearColor( bgColor );
    //camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    //camera->setRenderOrder( osg::Camera::PRE_RENDER );
    //camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    //camera->setViewport( 0, 0, sz.width, sz.height );
	//camera->setViewMatrixAsLookAt(osg::Vec3(sz.width/2.0f, sz.height/2.0f, sz.width/2.0f),
	//	                          osg::Vec3(sz.width/2.0f, sz.height/2.0f, 0),
	//							  osg::Vec3(0, 1, 0));
    //camera->addChild( scene );

	//viewer.getCamera()->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	//camera->attach(osg::Camera::COLOR_BUFFER, img);
	osg::Group* grp = new osg::Group;
	grp->addChild(scene);
	//grp->addChild(camera);
	scene->getOrCreateStateSet()->setAttributeAndModes(light, osg::StateAttribute::ON);
	osr(grp, &viewer, sz.width, sz.height, &res);
	cv::Mat temp(0, res.cols, res.type());
	for (int i = res.rows - 1; i >= 0; --i)
	{
		temp.push_back(res.row(i));
	}
	res = temp;
	//viewer.setSceneData(grp);
	//viewer.run();
	
	//viewer.setSceneData(grp);
	//viewer.realize();
	//viewer.frame();
	//viewer.setCameraManipulator(0);
	//viewer.run();
 	//cv::Mat wrap(sz.height, sz.width, CV_8UC3, img->data());
	//wrap.copyTo(res);
}

}

#endif