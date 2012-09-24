#include "topographic_to_locations.h"

#include <boost/property_tree/ptree.hpp>

#include <highgui.h>

namespace bpt = boost::property_tree;

namespace videoMosaic {

TopographicToLocations::TopographicToLocations(const bpt::ptree& ini)
{
	m_holeFillingIterations = ini.get("TopographicToLocations.HoleFillingIterations", 1);
}

namespace {

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

void TopographicToLocations::Process(const cv::Mat_<unsigned char>& topo, cv::Size tsize, IdealPolygonList& polygons)
{
	typedef std::vector<std::vector<cv::Point> > Contours;
	Contours contours;
	size_t rc = 0;
	cv::Mat_<unsigned char> topoCopy, locationsImage;
	topo.copyTo(topoCopy);
	cv::findContours(topoCopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	locationsImage = topoCopy;
	locationsImage.setTo(0);

	cv::Point ones(1,1);
	cv::Point tl, br;
	size_t nc = contours.size();
	for (size_t i = 0; i <  nc; ++i)
	{
		for (size_t j = 0; j < contours[i].size(); ++j)
		{
			cv::Point half(tsize.width - 1, tsize.height - 1);
			IdealPolygon poly;
			cv::Point thisPt = contours[i][j];
			tl = thisPt - half + ones;
			br = thisPt + half;
			tl = clamp(tl, locationsImage.size());
			br = clamp(br, locationsImage.size());
			if (cv::sum(locationsImage(cv::Rect(tl, br)))[0] > 0) 
			{
				continue;
			}
			locationsImage(thisPt) = 255;
			poly.center = clamp(thisPt, topo.size());
			poly.tileSize = tsize;
			polygons.push_back(poly);
		}
	}

	cv::Size currentSize = tsize - cv::Size(1,1);
	int minCounter = std::min(std::min(m_holeFillingIterations, tsize.width - 3), tsize.height - 3);
	for (int counter = 0; counter < minCounter; ++counter) 
	{
		cv::Mat_<float> lonleyPixels;
		cv::distanceTransform(255 - locationsImage, lonleyPixels, CV_DIST_C, 3);
		cv::threshold(lonleyPixels, lonleyPixels, (currentSize.width + currentSize.height) / 2 , 1.0f, CV_THRESH_BINARY);
		Skeleton(lonleyPixels,lonleyPixels);
		cv::Mat_<unsigned char> ucLonely;
		lonleyPixels.convertTo(ucLonely, CV_8UC1);

		for (int r = 0; r < ucLonely.rows; ++r)
		{
			for (int c = 0; c < ucLonely.cols; ++c)
			{
				cv::Point half(currentSize.width - 1, currentSize.height - 1);
				IdealPolygon poly;
				cv::Point thisPt(c,r);
				if (ucLonely(thisPt) != 1)
				{
					continue;
				}
				tl = thisPt - half + ones;
				br = thisPt + half;
				tl = clamp(tl, locationsImage.size());
				br = clamp(br, locationsImage.size());
				if (cv::sum(locationsImage(cv::Rect(tl, br)))[0] > 0) 
				{
					continue;
				}
				locationsImage(thisPt) = 255;
				poly.center = clamp(thisPt, topo.size());
				poly.tileSize = currentSize;
				polygons.push_back(poly);
			}
		}
		currentSize = currentSize - cv::Size(1,1);
	}

}

}