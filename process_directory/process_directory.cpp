#include <cv.h>
#include <highgui.h>

#include "directory_source.h"
#include "video_to_mosaic.h"
#include "parameters_parser.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using namespace cv;

int main(int argc, char** argv)
{
	Mat_<cv::Vec3b> frame, fcolor;

	videoMosaic::DirectorySource ds(".", "frame(\\d+)\\.png",0,0);
	boost::property_tree::ptree ini;
	std::vector<std::string> args;
	videoMosaic::ParseParameters(argc - 1, argv + 1, ini, args);
	int skip = ini.get("ProcessDirectory.Skip", 0);
	for (int i = 0; i < skip; ++i) ds.Next();
	
	videoMosaic::VideoToMosaic vtm(ini);
	while (ds.HasNext())
	{
		frame = ds.Next();
		std::string inName = boost::filesystem::path(ds.GetName()).filename().string();
		std::cout << inName << std::endl;
		std::string outName = "vm_" + inName;
		vtm.ProcessNext(frame, fcolor);
		
		cv::imwrite(outName, fcolor);
	}
	return 0;
}