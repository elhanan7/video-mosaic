//#include <boost/test/unit_test.hpp>
//
//#include <cv.h>
//#include <highgui.h>
//
//#include "image_to_mosaic.h"
//
//#include <boost/property_tree/ini_parser.hpp>
//#include <boost/filesystem.hpp>
//
//using namespace cv;
//
//BOOST_AUTO_TEST_CASE(process_directory)
//{
//	Mat_<cv::Vec3b> frame, fcolor;
//	Mat edges;
//
//	cv::Mat_<unsigned char> final;
//	DirectorySource ds(".", "frame(\\d+)\\.png",0,0);
//
//	boost::property_tree::ptree ini;
//	boost::property_tree::ini_parser::read_ini("C:\\Users\\elhanan7\\Programming\\CVM\\Data\\INI\\cvm.ini", ini);
//	VideoToMosaic vtm(ini);
//	bool first = true;
//	while (ds.HasNext())
//	{
//		frame = ds.Next();
//		std::string inName = boost::filesystem::path(ds.GetName()).filename().string();
//		std::cout << inName << std::endl;
//		std::string outName = "vm_" + inName;
//		std::string glName = "gl_" + outName;
//		std::string topoName = "topo_" + outName;
//		
//		vtm.ProcessNext(frame, fcolor);
//		//boost::unit_test::framework::master_test_suite().argv;
//		
//		cv::imwrite(outName, fcolor);
//	}	
//}