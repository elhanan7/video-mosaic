#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

#include <cv.h>
#include <highgui.h>

#include "image_to_mosaic.h"

namespace videoMosaic { namespace python {

	bool numpy_to_mat(const PyObject* o, cv::Mat& m, bool allowND = true);
	bool mat_to_numpy(const cv::Mat& m, PyObject** o);

	class VideoMosaicParameters
	{
	public:
		VideoMosaicParameters();
		VideoMosaicParameters(const std::string& iniFile);

		void Set(const std::string& param, const boost::python::object& obj);
		const std::string Get(const std::string& param);

		boost::property_tree::ptree m_ptree;
	};

	class PythonImageToMosaic
	{
	public:
		PythonImageToMosaic() : itm(params.m_ptree) {};
		PythonImageToMosaic(const VideoMosaicParameters& params_) : params(params_), itm(params.m_ptree) {};

		PyObject* ProcessFile(const std::string& fname);

		VideoMosaicParameters params;
		ImageToMosaic itm;

	};

} }