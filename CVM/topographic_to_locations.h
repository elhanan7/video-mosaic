#pragma once

#include <cv.h>
#include <vector>

#include <boost/property_tree/ptree_fwd.hpp>

#include "utils.h"
#include "ideal_to_cut_polygons.h"

namespace videoMosaic {

class TopographicToLocations
{
public:

	TopographicToLocations(const boost::property_tree::ptree& ini);

	void Process(const cv::Mat_<unsigned char>& topo,const cv::Mat_<float> dx, const cv::Mat_<float> dy ,cv::Size2f tsize, PolygonList& polygons);

private:

	template <typename Traits>
	void ProcessInternal(Traits& traits, const cv::Mat_<unsigned char>& topo, cv::Size2f tsize, PolygonList& polygons);


	IdealToCutPolygon m_itc;
	int m_holeFillingIterations;
	bool m_precise;
	float m_maxOverlap;
};

}


