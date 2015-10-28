#include <cv.h>
#include <highgui.h>
#include <opencv2/stitching/stitcher.hpp>
#include <boost/property_tree/ptree.hpp>
#include "directory_source.h"
#include "scene_to_mosaic.h"
#include "parameters_parser.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using namespace cv;

int main(int argc, char** argv)
{
	Mat_<cv::Vec3b> frame, fcolor;

	boost::property_tree::ptree ini;
	std::vector<std::string> args;
	videoMosaic::ParseParameters(argc - 1, argv + 1, ini, args);
	int skip = ini.get("StitchTest.Skip", 0);
   int length = ini.get("StitchTest.Length", 30);
	bool doReverse = ini.get("ProcessDirectory.Reverse", 0) != 0;
	videoMosaic::DirectorySource ds(".", "frame(\\d+)\\.png", 0, 0, doReverse);
	for (int i = 0; i < skip; ++i) ds.Next();
	
	videoMosaic::SceneToMosaic stm(ini);
   std::vector<std::string> imgNames;
   int idx{0};
   while (ds.HasNext())
	{
      ++idx;
		frame = ds.Next();
		stm.ProcessNext(frame);
      std::string inName = boost::filesystem::path(ds.GetName()).filename().string();
		std::cout << inName << std::endl;
      imgNames.push_back(inName);
      if (idx > length - 1) break;
	}
   stm.ProcessAll();
   for (const auto& imgName: imgNames)
   {
      cv::Mat res = stm.ReceiveNext();
      cv::imwrite("vm_" + imgName, res);
      std::cout << "Wrote " << "vm_" + imgName << std::endl;
   }
    return 0;
}