#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/shared_ptr.hpp>

#include "image_to_mosaic.h"

class VideoToMosaic
{
public:
	VideoToMosaic(const boost::property_tree::ptree& ini);

private:
	boost::shared_ptr<ImageToMosaic> m_imageToMosaic;

};

