#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

#include <cv.h>

namespace videoMosaic { namespace python {

	bool numpy_to_mat(const PyObject* o, cv::Mat& m, bool allowND = true);

	class VideoMosaicParameters
	{
	public:
		VideoMosaicParameters();
		VideoMosaicParameters(const std::string& iniFile);

		void Set(const std::string& param, const boost::python::object& obj);
		const std::string Get(const std::string& param);

		boost::property_tree::ptree m_ptree;
	};

} }