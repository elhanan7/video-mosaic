#include "cvm_python_classes.h"

#include <boost/python.hpp>
#include <boost/python/numeric.hpp>

#include <iostream>

using namespace boost::python;
using namespace videoMosaic::python;

void TestArray(boost::python::numeric::array a)
{
	cv::Mat m;
	numpy_to_mat(a.ptr(), m);
	std::cout << m.at<int>(cv::Point(0,1))<< std::endl;
}

PyObject* TestArray2()
{
	cv::Mat m = cv::Mat::ones(10,10,CV_8U);
	m.at<unsigned char>(cv::Point(5,5)) = 128;
	PyObject* o;
	mat_to_numpy(m, &o);
	return o;
}

BOOST_PYTHON_MODULE(video_mosaic)
{
	boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

	class_<PythonImageToMosaic>("ImageToMosaic", init<const std::string&>())
		.def(init<>())
		.def("ProcessFile", &PythonImageToMosaic::ProcessFile)
		.def("Process", &PythonImageToMosaic::Process)
		.def("GetParam", &PythonImageToMosaic::GetParam)
		.def("SetParam", &PythonImageToMosaic::SetParam);


	def("Test", &TestArray);
	def("Test2", &TestArray2);


}