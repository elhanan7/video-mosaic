#include "image_to_features.h"

#include <boost/property_tree/ini_parser.hpp>

using namespace boost::property_tree;

ImageToFeatures::ImageToFeatures(const ptree& ini)
{
	m_numOfFeatures = ini.get("ImageToFeatures.NumberOfFeatures", 200);
	m_minimalMatchDistance = ini.get("ImageToFeatures.Distance", 150.0f);
}

ImageToFeatures::~ImageToFeatures(void)
{
}
