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
BOOST_PYTHON_MODULE(video_mosaic)
{
	boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

	class_<VideoMosaicParameters>("Parameters", init<std::string>())
		.def(init<>())
		.def("Get", &VideoMosaicParameters::Get)
		.def("Set", &VideoMosaicParameters::Set);

	def("Test", &TestArray);


}