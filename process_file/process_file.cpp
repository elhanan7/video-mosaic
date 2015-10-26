#include <cv.h>
#include <highgui.h>

#include "image_to_mosaic.h"
#include "parameters_parser.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/regex.hpp>
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
   std::vector<std::string> args;
   videoMosaic::ParseParameters(argc - 1, argv + 1, ini, args);
   videoMosaic::ImageToMosaic itm(ini);
   std::string inName = boost::filesystem::path(args[0]).filename().string();
   frame = cv::imread(std::string(argv[1]));

   std::cout << inName << std::endl;
   std::string outName = "vm_" + inName;
   std::string glName = "gl_" + inName;
   std::string topoName = "topo_" + inName;
   if (args.size() > 1)
   {
      outName = args[1];
      glName = "gl_" + args[1];
      topoName = "topo_" + args[1];
   }

   itm.Process(frame, fcolor);

   cv::imwrite(outName, fcolor);
   if (ini.get("ProcessFile.SaveGL", 0) == 1)
   {
      cv::imwrite(glName, itm.GetGuideLinesImage());
   }
   if (ini.get("ProcessFile.SaveTopological", 0) == 1)
   {
      cv::imwrite(topoName, itm.GetTopologicalImage());
   }

   return 0;
}