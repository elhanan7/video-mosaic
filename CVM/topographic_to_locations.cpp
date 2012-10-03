#include "topographic_to_locations.h"
#include "transfromations.h"

#include <boost/property_tree/ptree.hpp>

#include <highgui.h>

#include "precise_tile_placer.h"
#include "approximate_tile_placer.h"

namespace bpt = boost::property_tree;

namespace videoMosaic {

TopographicToLocations::TopographicToLocations(const bpt::ptree& ini) : m_itc(ini)
{
	m_holeFillingIterations = ini.get("TopographicToLocations.HoleFillingIterations", 1);
	m_precise = ini.get("TopographicToLocations.PreciseTilePlacing", false);
	m_maxOverlap = ini.get("TopographicToLocations.MaxOverlapPercent", 0.1f);
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

template <typename PointSource, typename Traits>
void DoLocations(const PointSource& ps, Traits& traits, PolygonList& polygons)
{
	typedef typename Traits::PolygonType PolygonType;
	typedef typename PointSource::IteratorType IteratorType;
	IteratorType iter  = ps.Begin();
	for (; iter != ps.End(); ++iter)
	{
		PolygonType polygon = traits.GetPolygon(*iter);
		traits.CheckUpdate(polygon, polygons);
	}
}

struct ContourSource
{
	ContourSource(cv::Mat_<unsigned char> topo)
	{
		typedef std::vector<std::vector<cv::Point> > Contours;
		Contours contours;
		size_t rc = 0;
		cv::Mat_<unsigned char> topoCopy;
		topo.copyTo(topoCopy);
		cv::findContours(topoCopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		size_t nc = contours.size();
		for (size_t i = 0; i <  nc; ++i)
		{
			for (size_t j = 0; j < contours[i].size(); ++j)
			{
				container.push_back(contours[i][j]);
			}
		}
	}

	typedef std::vector<cv::Point>::const_iterator IteratorType;

	IteratorType Begin() const {return container.begin(); } 
	IteratorType End()  const {return container.end(); } 

	std::vector<cv::Point> container;
};

struct HolesSource
{
	HolesSource(cv::Mat_<unsigned char> mask, cv::Size2f tsize)
	{
		cv::Mat_<float> lonleyPixels;
		cv::distanceTransform(255 - mask, lonleyPixels, CV_DIST_C, 3);
		cv::threshold(lonleyPixels, lonleyPixels, (tsize.width + tsize.height) / 4 , 1.0f, CV_THRESH_BINARY);
		Skeleton(lonleyPixels,lonleyPixels);
		cv::Mat_<unsigned char> ucLonely;
		lonleyPixels.convertTo(ucLonely, CV_8UC1);

		for (int r = 0; r < ucLonely.rows; ++r)
		{
			for (int c = 0; c < ucLonely.cols; ++c)
			{
				cv::Point thisPt(c,r);
				if (ucLonely(thisPt) == 1)
				{
					container.push_back(thisPt);
				}
			}
		}
	}

	typedef std::vector<cv::Point>::const_iterator IteratorType;

	IteratorType Begin() const {return container.begin(); }
	IteratorType End() const {return container.end(); }

	std::vector<cv::Point> container;
};
}

template <typename Traits>
void TopographicToLocations::ProcessInternal(Traits& traits, const cv::Mat_<unsigned char>& topo, cv::Size2f tsize, PolygonList& polygons)
{
	ContourSource contourSource(topo);

	DoLocations(contourSource, traits, polygons);

	cv::Size2f currentSize = tsize - cv::Size2f(1,1);
	int minCounter = std::min(std::min(this->m_holeFillingIterations, static_cast<int>(tsize.width) - 2), 
		                                 static_cast<int>(tsize.height) - 2);
	for (int counter = 0; counter < minCounter; ++counter) 
	{
		traits.SetTileSize(currentSize);
		HolesSource holesSource(traits.GetMask(), currentSize);
		DoLocations(holesSource, traits, polygons);
		currentSize = currentSize - cv::Size2f(1,1);
	}

	if (Traits::NeedsClipping())
	{
		PolygonList cutPolygons;
		this->m_itc.Process(polygons, tsize, topo.size(), cutPolygons);
		polygons = cutPolygons;
	}
}

void TopographicToLocations::Process(const cv::Mat_<unsigned char>& topo,const cv::Mat_<float> dx, const cv::Mat_<float> dy ,cv::Size2f tsize, PolygonList& polygons)
{
	if (m_precise)
	{
		PrecisePlacerTraits precise(dx, dy, tsize, m_maxOverlap);
		ProcessInternal(precise, topo, tsize, polygons);
	}
	else
	{
		ApproximatePlacerTraits approx(topo.size(), tsize, dx, dy);
		ProcessInternal(approx, topo, tsize, polygons);
	}
}

}