#include "topographic_to_locations.h"

#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

TopographicToLocations::TopographicToLocations(const bpt::ptree& ini)
{
	m_tsize = ini.get("ImageToMosaic.TileSize", 4);
}

namespace {
cv::Point  clamp(const cv::Point& in, int clampx,int clampy)
{
	int rx = std::min(clampx, std::max(in.x,0));
	int ry = std::min(clampy, std::max(in.y,0));
	return cv::Point(rx, ry);
}
}

void TopographicToLocations::Process(const cv::Mat_<unsigned char>& topo, LocationList& locations)
{
	typedef std::vector<std::vector<cv::Point> > Contours;
	Contours contours;
	size_t rc = 0;
	cv::Mat_<unsigned char> topoCopy;
	topo.copyTo(topoCopy);
	cv::findContours(topoCopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	topoCopy.setTo(0);
	size_t nc = contours.size();
	for (size_t i = 0; i <  nc; ++i)
	{
		cv::Point oldPt(-99,-99);
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			cv::Point thisPt = contours[i][j];
			cv::Point half(m_tsize / 2 , m_tsize / 2 );
			cv::Point tl, br;
			tl = thisPt - half;
			br = thisPt + half;
			tl = clamp(tl, topoCopy.cols - 1, topoCopy.rows - 1);
			br = clamp(br, topoCopy.cols - 1, topoCopy.rows - 1);
			if (cv::sum(topoCopy(cv::Rect(tl, br)))[0] > 0) 
			{
				continue;
			}
			topoCopy(thisPt) = 255;
			locations.push_back(thisPt);
		}
	}
}