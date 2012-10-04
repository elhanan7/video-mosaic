#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
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
		virtual ~VideoMosaicParameters() {};
		VideoMosaicParameters(const std::string& iniFile);

		void SetParam(const std::string& param, const boost::python::object& obj);
		const std::string GetParam(const std::string& param);
	protected:
		virtual void OnParamChange() = 0;

		boost::property_tree::ptree m_ptree;
	};

	class PythonImageToMosaic : public VideoMosaicParameters
	{
	public:
		PythonImageToMosaic() {OnParamChange();};
		PythonImageToMosaic(const std::string& iniFile) : VideoMosaicParameters(iniFile) {OnParamChange();};

		PyObject* ProcessFile(const std::string& fname);
		PyObject* Process(const boost::python::numeric::array arr);
	protected:
		virtual void OnParamChange();

		boost::shared_ptr<ImageToMosaic> itm;

	};

} }