#include "topographic_to_locations.h"

#include <boost/property_tree/ptree.hpp>

#include <highgui.h>

namespace bpt = boost::property_tree;

TopographicToLocations::TopographicToLocations(const bpt::ptree& ini)
{
	m_tsize = ini.get("ImageToMosaic.TileSize", 4);
	m_holeFillingIterations = ini.get("TopographicToLocations.HoleFillingIterations", 1);
}

namespace {
cv::Point  clamp(const cv::Point& in, int clampx,int clampy)
{
	int rx = std::min(clampx, std::max(in.x,0));
	int ry = std::min(clampy, std::max(in.y,0));
	return cv::Point(rx, ry);
}

void Skeleton(cv::Mat_<unsigned char> input, cv::Mat& output)
{
	cv::Mat img = input.clone();

	cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
	cv::Mat temp;
	cv::Mat eroded;
 
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
 
	bool done;		
	do
	{
	  cv::erode(img, eroded, element);
	  cv::dilate(eroded, temp, element); // temp = open(img)
	  cv::subtract(img, temp, temp);
	  cv::bitwise_or(skel, temp, skel);
	  eroded.copyTo(img);
 
	  done = (cv::norm(img) == 0);
	} while (!done);

	output = skel;
}
}

void TopographicToLocations::Process(const cv::Mat_<unsigned char>& topo, LocationList& locations)
{
	typedef std::vector<std::vector<cv::Point> > Contours;
	Contours contours;
	size_t rc = 0;
	cv::Mat_<unsigned char> topoCopy, locationsImage;
	topo.copyTo(topoCopy);
	cv::findContours(topoCopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	locationsImage = topoCopy;
	locationsImage.setTo(0);

	cv::Point half(m_tsize - 1, m_tsize - 1);
	cv::Point ones(1,1);
	cv::Point tl, br;
	size_t nc = contours.size();
	for (size_t i = 0; i <  nc; ++i)
	{
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			cv::Point thisPt = contours[i][j];
			tl = thisPt - half + ones;
			br = thisPt + half;
			tl = clamp(tl, locationsImage.cols - 1, locationsImage.rows - 1);
			br = clamp(br, locationsImage.cols - 1, locationsImage.rows - 1);
			if (cv::sum(locationsImage(cv::Rect(tl, br)))[0] > 0) 
			{
				continue;
			}
			locationsImage(thisPt) = 255;
			locations.push_back(thisPt);
		}
	}

	for (int counter = 0; counter < m_holeFillingIterations; ++counter) 
	{
		cv::Mat_<float> lonleyPixels;
		cv::distanceTransform(255 - locationsImage, lonleyPixels, CV_DIST_C, 3);
		cv::threshold(lonleyPixels, lonleyPixels, m_tsize / 2 , 1.0f, CV_THRESH_BINARY);
		Skeleton(lonleyPixels,lonleyPixels);
		cv::Mat_<unsigned char> ucLonely;
		lonleyPixels.convertTo(ucLonely, CV_8UC1);

		for (int r = 0; r < ucLonely.rows; ++r)
		{
			for (int c = 0; c < ucLonely.cols; ++c)
			{
				cv::Point thisPt(c,r);
				if (ucLonely(thisPt) != 1)
				{
					continue;
				}
				tl = thisPt - half + ones;
				br = thisPt + half;
				tl = clamp(tl, locationsImage.cols - 1, locationsImage.rows - 1);
				br = clamp(br, locationsImage.cols - 1, locationsImage.rows - 1);
				if (cv::sum(locationsImage(cv::Rect(tl, br)))[0] > 0) 
				{
					continue;
				}
				locationsImage(thisPt) = 255;
				locations.push_back(thisPt);
			}
		}
	}

}