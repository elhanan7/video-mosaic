#include <cv.h>
#include <highgui.h>

#include "image_to_mosaic.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

using namespace cv;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "You must supply an image to process." << std::endl;
		return 1;
	}
	Mat_<cv::Vec3b> frame, fcolor;

	boost::property_tree::ptree ini;
	boost::property_tree::ini_parser::read_ini("C:\\Users\\elhanan7\\Programming\\CVM\\Data\\INI\\cvm.ini", ini);
	videoMosaic::ImageToMosaic itm(ini);
	std::string inName = boost::filesystem::path(argv[1]).filename().string();
	frame = cv::imread(inName);

	std::cout << inName << std::endl;
	std::string outName = "vm_" + inName;
	if (argc > 2)
	{
		outName = std::string(argv[2]);
	}

	itm.Process(frame, fcolor);

	cv::imwrite(outName, fcolor);

	return 0;
}