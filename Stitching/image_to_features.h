#pragma once

#include <boost/property_tree/ptree.hpp>

class ImageToFeatures
{
public:
	ImageToFeatures(const boost::property_tree::ptree& ini = boost::property_tree::ptree());
	~ImageToFeatures();
	
private:
	unsigned int m_numOfFeatures;
	float m_minimalMatchDistance;
};

