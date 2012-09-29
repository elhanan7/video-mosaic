#include "topographic_to_locations.h"
#include "transfromations.h"

#include <boost/property_tree/ptree.hpp>

#include <highgui.h>

namespace bpt = boost::property_tree;

namespace videoMosaic {

TopographicToLocations::TopographicToLocations(const bpt::ptree& ini)
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
void DoLocations(const PointSource& ps, Traits& traits, IdealPolygonList& polygons)
{
	typedef typename Traits::PolygonType PolygonType;
	typedef typename PointSource::IteratorType IteratorType;
	IteratorType iter  = ps.Begin();
	for (; iter != ps.End(); ++iter)
	{
		PolygonType polygon = traits.GetPolygon(*iter);
		if (traits.CheckUpdate(polygon))
		{
			polygons.push_back(traits.ExtractIdeal(polygon));
		}
	}
}

struct ApproximateTraits
{
	ApproximateTraits(cv::Size imsz, cv::Size tsz) : imSize(imsz), tSize(tsz) 
	{
		mask = cv::Mat_<unsigned char>::zeros(imSize);
		half = cv::Point(tSize.width - 1, tSize.height - 1);
		ones = cv::Point(1,1);
	};
	
	typedef cv::Point PolygonType;

	PolygonType GetPolygon(cv::Point pt)
	{
		return pt;
	}

	void SetTileSize(cv::Size tsz)  {tSize = tsz;}

	bool CheckUpdate(const cv::Point& pt)
	{

		cv::Point tl = pt - half + ones;
		cv::Point br = pt + half;
		tl = clamp(tl, imSize);
		br = clamp(br, imSize);
		if (cv::sum(mask(cv::Rect(tl, br)))[0] > 0) 
		{
			return false;
		}
		
		mask(pt) = 255;
		return true;
	}

	IdealPolygon ExtractIdeal(const cv::Point& pt)
	{
		IdealPolygon res;
		res.center = pt;
		res.tileSize = tSize;
		return res;
	}

	cv::Mat_<unsigned char> GetMask()
	{
		return mask;
	}

	cv::Size imSize, tSize;
	cv::Mat_<unsigned char> mask;
	cv::Point half, ones;
};

struct PreciseTraits
{
	PreciseTraits(cv::Mat_<float> dxx, cv::Mat_<float> dyy, cv::Size tsz, float maxAreaPercent) : imSize(dxx.size()), tSize(tsz), dx(dxx), dy(dyy),
		maxPercent(maxAreaPercent)
	{
		mask = cv::Mat_<unsigned char>::zeros(imSize);
		half = cv::Point(tSize.width - 1, tSize.height - 1);
		ones = cv::Point(1,1);
	};
	
	typedef Polygon PolygonType;

	PolygonType GetPolygon(cv::Point pt)
	{
		Polygon poly;
		poly.ideal.center = pt;
		poly.ideal.orientation = atan2(dy(pt), dx(pt));
		poly.ideal.tileSize = tSize;

		std::vector<cv::Point2d> temp;
		double halfx = poly.ideal.tileSize.width / 2;
		double halfy = poly.ideal.tileSize.height / 2.0;
		poly.polygon.push_back(cv::Point2d(-halfx, -halfy));
		poly.polygon.push_back(cv::Point2d( halfx, -halfy));
		poly.polygon.push_back(cv::Point2d( halfx,  halfy));
		poly.polygon.push_back(cv::Point2d(-halfx,  halfy));
		transformations::Shift shift(poly.ideal.center.x ,poly.ideal.center.y);
		transformations::Rotate rot(poly.ideal.orientation);
		std::transform(poly.polygon.begin(), poly.polygon.end(), poly.polygon.begin(), shift*rot);
		return poly;
	}

	bool CheckUpdate(const PolygonType& poly)
	{
		singlePolyMask.create(imSize);
		singlePolyMask.setTo(0);
		std::vector<cv::Point2i> intVec;
		std::transform(poly.polygon.cbegin(), poly.polygon.cend(), std::back_inserter(intVec), [&](const cv::Point2d& dpt) -> cv::Point
		{
			cv::Point intPt(cv::saturate_cast<int>(dpt.x), cv::saturate_cast<int>(dpt.y));

			return clamp(intPt, this->imSize);
		});
		cv::fillConvexPoly(singlePolyMask, intVec, cv::Scalar(255));
		cv::Rect roi(poly.ideal.center - cv::Point(tSize.width, tSize.height), tSize * 2);
		cv::Rect fullRoi(cv::Point(0,0), imSize);
		roi = roi & fullRoi;
		cv::Mat_<unsigned char> maskRoi, singlePolyRoi, roiResult;
		maskRoi = mask(roi);
		singlePolyRoi = singlePolyMask(roi);
		cv::bitwise_and(maskRoi, singlePolyRoi, roiResult);
		int n = cv::saturate_cast<int>(cv::sum(roiResult)[0] / 255);
		float percent = (float) n / roi.area();

		if (percent > maxPercent)
		{
			return false;
		}
		cv::bitwise_or(maskRoi, singlePolyRoi, maskRoi);
		return true;
	}

	void SetTileSize(cv::Size tsz)  {tSize = tsz;}

	IdealPolygon ExtractIdeal(const PolygonType& poly)
	{
		return poly.ideal;
	}

	cv::Mat_<unsigned char> GetMask()
	{
		return mask;
	}

	cv::Size imSize, tSize;
	cv::Mat_<float> dx,dy;
	float maxPercent;
	cv::Mat_<unsigned char> mask, singlePolyMask;
	cv::Point half, ones;
};

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
	HolesSource(cv::Mat_<unsigned char> mask, cv::Size tsize)
	{
		cv::Mat_<float> lonleyPixels;
		//cv::imshow("mask", mask);
		//cv::waitKey();
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

template <typename Traits>
void MainAlgorithm(Traits& traits, const cv::Mat_<unsigned char>& topo, cv::Size tsize, IdealPolygonList& polygons, int holeFillingIters)
{
	ContourSource contourSource(topo);

	DoLocations(contourSource, traits, polygons);

	cv::Size currentSize = tsize - cv::Size(1,1);
	int minCounter = std::min(std::min(holeFillingIters, tsize.width - 2), tsize.height - 2);
	for (int counter = 0; counter < minCounter; ++counter) 
	{
		traits.SetTileSize(currentSize);
		HolesSource holesSource(traits.GetMask(), currentSize);
		DoLocations(holesSource, traits, polygons);
		currentSize = currentSize - cv::Size(1,1);
	}
}
}

void TopographicToLocations::Process(const cv::Mat_<unsigned char>& topo,const cv::Mat_<float> dx, const cv::Mat_<float> dy ,cv::Size tsize, IdealPolygonList& polygons)
{
	//cv::namedWindow("mask");
	if (m_precise)
	{
		PreciseTraits precise(dx, dy, tsize, m_maxOverlap);
		MainAlgorithm(precise, topo, tsize, polygons, m_holeFillingIterations);
	}
	else
	{
		ApproximateTraits approx(topo.size(), tsize);
		MainAlgorithm(approx, topo, tsize, polygons, m_holeFillingIterations);
	}
}

}