#include <Windows.h>
#include <osg/Camera>
#include <osg/CameraNode>
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>

class SnapImageDrawCallback : public ::osg::CameraNode::DrawCallback 
{ 
public:
	SnapImageDrawCallback() 
	{ 
		_snapImageOnNextFrame = false; 
	}

	void setFileName(const std::string& filename) { _filename = filename; } 
	const std::string& getFileName() const { return _filename; }
	void setSnapImageOnNextFrame(bool flag) { _snapImageOnNextFrame = flag; } 
	bool getSnapImageOnNextFrame() const { return _snapImageOnNextFrame; }
	virtual void operator () (const ::osg::CameraNode& camera) const 
	{ 
		::osg::notify(::osg::NOTICE) << "Saving screen image to '"<<_filename<<"'"<< std::endl; 
		if (!_snapImageOnNextFrame) return;
		int x,y,width,height; 
		x = camera.getViewport()->x(); 
		y = camera.getViewport()->y(); 
		width = camera.getViewport()->width(); 
		height = camera.getViewport()->height();
		::osg::ref_ptr< ::osg::Image> image = new ::osg::Image(); 
		image->readPixels(x,y,width,height,GL_RGB,GL_UNSIGNED_BYTE);

		if (::osgDB::writeImageFile(*image,_filename)) 
		{ 
			std::cout << "Saved screen image to '"<<_filename<<"'"<< std::endl; 
		}
		_snapImageOnNextFrame = false; 
	}

protected:

	::std::string _filename; 
	mutable bool _snapImageOnNextFrame;

}; 

typedef unsigned int GLenum;

void 
renderSceneToImage(::osgViewer::Viewer* viewer, const ::std::string& sFileName_, int width, int height)
{
	int nWidth = width, nHeight = height;

	viewer->getCamera()->setRenderTargetImplementation(::osg::CameraNode::FRAME_BUFFER_OBJECT);

	::osg::ref_ptr<SnapImageDrawCallback> snapImageDrawCallback = new SnapImageDrawCallback(); 
	viewer->getCamera()->setPostDrawCallback (snapImageDrawCallback.get());

	snapImageDrawCallback->setFileName(sFileName_); 
	snapImageDrawCallback->setSnapImageOnNextFrame(true); 


	::osg::ref_ptr< ::osg::GraphicsContext> pbuffer;

	::osg::ref_ptr< ::osg::GraphicsContext::Traits> traits = new ::osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = nWidth;
	traits->height = nHeight;
	traits->red = 8;
	traits->green = 8;
	traits->blue = 8;
	traits->alpha = 8;
	traits->windowDecoration = false;
	traits->pbuffer = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	pbuffer = ::osg::GraphicsContext::createGraphicsContext(traits.get());
	if (pbuffer.valid())
	{ 
		::osg::ref_ptr< ::osg::Camera> camera2 = new ::osg::Camera();
		camera2->setGraphicsContext(pbuffer.get());
		GLenum buffer = pbuffer->getTraits()->doubleBuffer ? GL_BACK : GL_FRONT;
		camera2->setDrawBuffer(buffer);
		camera2->setReadBuffer(buffer);
		camera2->setViewport(viewer->getCamera()->getViewport());
		viewer->addSlave(camera2.get(), ::osg::Matrixd(), ::osg::Matrixd());
	}

	viewer->realize();
	viewer->frame();
}