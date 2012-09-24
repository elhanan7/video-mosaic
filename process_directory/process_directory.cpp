#include <cv.h>
#include <highgui.h>

#include "directory_source.h"
#include "video_to_mosaic.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

using namespace cv;

int main()
{
	Mat_<cv::Vec3b> frame, fcolor;
	Mat edges;

	cv::Mat_<unsigned char> final;
	videoMosaic::DirectorySource ds(".", "frame(\\d+)\\.png",0,0);

	boost::property_tree::ptree ini;
	boost::property_tree::ini_parser::read_ini("C:\\Users\\elhanan7\\Programming\\CVM\\Data\\INI\\cvm.ini", ini);
	videoMosaic::VideoToMosaic vtm(ini);
	bool first = true;
	while (ds.HasNext())
	{
		frame = ds.Next();
		std::string inName = boost::filesystem::path(ds.GetName()).filename().string();
		std::cout << inName << std::endl;
		std::string outName = "vm_" + inName;
		std::string glName = "gl_" + outName;
		std::string topoName = "topo_" + outName;
		
		vtm.ProcessNext(frame, fcolor);
		//boost::unit_test::framework::master_test_suite().argv;
		
		cv::imwrite(outName, fcolor);
	}
	return 0;
}