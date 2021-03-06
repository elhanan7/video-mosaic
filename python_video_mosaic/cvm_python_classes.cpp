#include "cvm_python_classes.h"

#include <boost/property_tree/ini_parser.hpp>

namespace videoMosaic { namespace python {

	VideoMosaicParameters::VideoMosaicParameters() {}

	VideoMosaicParameters::VideoMosaicParameters(const std::string& iniFile)
	{
		boost::property_tree::ini_parser::read_ini(iniFile, m_ptree);
	}

	void VideoMosaicParameters::SetParam(const std::string& param, const boost::python::object& obj)
	{
		std::string value = boost::python::extract<std::string>(obj.attr("__str__")());
		m_ptree.put(param, value);
		OnParamChange();
	}

	const std::string VideoMosaicParameters::GetParam(const std::string& param)
	{
		return m_ptree.get<std::string>(param);
	}

	PyObject* PythonImageToMosaic::ProcessFile(const std::string& fname)
	{
		cv::Mat m = cv::imread(fname);
		cv::Mat_<cv::Vec3b> out;
		itm->Process(m, out);
		cv::Mat outMat(out);
		PyObject* o;
		mat_to_numpy(outMat, &o);
		return o;
	}

	PyObject* PythonImageToMosaic::Process(const boost::python::numeric::array arr)
	{
		cv::Mat m;
		numpy_to_mat(arr.ptr(), m, true);
		cv::Mat_<cv::Vec3b> out;
		itm->Process(m, out);
		cv::Mat outMat(out);
		PyObject* o;
		mat_to_numpy(outMat, &o);
		return o;
	}

	void PythonImageToMosaic::OnParamChange()
	{
		itm = boost::shared_ptr<ImageToMosaic>(new ImageToMosaic(m_ptree));
	}


} }