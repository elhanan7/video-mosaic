#include "cvm_python_classes.h"

#include <boost/property_tree/ini_parser.hpp>

namespace videoMosaic { namespace python {

	VideoMosaicParameters::VideoMosaicParameters() {}

	VideoMosaicParameters::VideoMosaicParameters(const std::string& iniFile)
	{
		boost::property_tree::ini_parser::read_ini(iniFile, m_ptree);
	}

	void VideoMosaicParameters::Set(const std::string& param, const boost::python::object& obj)
	{
		std::string value = boost::python::extract<std::string>(obj.attr("__str__")());
		m_ptree.put(param, value);
	}

	const std::string VideoMosaicParameters::Get(const std::string& param)
	{
		return m_ptree.get<std::string>(param);
	}


} }