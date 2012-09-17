#pragma once

namespace osg {class Node;}
#include <cv.h>

class LocationsToScene
{
public:
	typedef std::vector<cv::Point> PointList;
	typedef std::vector<float> OrientationList;
	LocationsToScene() : m_tsize(4) {};
	void Process();
	osg::Node* Process(const cv::Mat_<cv::Vec3b>& clrImage, const PointList& centers, const OrientationList& os);
	void SetTileSize(float tsize) {m_tsize = tsize;}
	float GetTileSize() {return m_tsize;}
private:
	float m_tsize;
};

