#include <cv.h>
#include <highgui.h>
#include <osgViewer/Viewer>
#include <osg/Node>

#include "image_to_mosaic.h"
#include "directory_source.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

using namespace cv;

const int TSIZE(4);

int main(int argc, char** argv)
{
	Mat_<cv::Vec3b> frame, fcolor;
	Mat edges;

	cv::Mat_<unsigned char> final;
	//std::string inName = "C:\\Users\\elhanan7\\Programming\\CVM\\Data\\CANNON\\frame001.png";
	//std::string inName = "C:\\Users\\elhanan7\\Programming\\CVM\\Data\\IMAGES\\bluedragon.jpg";
	//std::string outName = "res.png";
	//if (argc > 1)
	//{
	//	inName = argv[1];
	//	outName = "vm_" + inName;
	//}
	DirectorySource ds(".", "frame(\\d+)\\.png",0,0);

	boost::property_tree::ptree ini;
	boost::property_tree::ini_parser::read_ini("C:\\Users\\elhanan7\\Programming\\CVM\\Data\\INI\\cvm.ini", ini);
	ImageToMosaic itm(ini);

	while (ds.HasNext())
	{
		frame = ds.Next();
		std::string inName = boost::filesystem::path(ds.GetName()).filename().string();
		std::cout << inName << std::endl;
		std::string outName = "vm_" + inName;
		std::string glName = "gl_" + outName;
		std::string topoName = "topo_" + outName;
		//frame = imread("C:\\Users\\elhanan7\\Documents\\dragon1.jpg");
		//cap >> frame;
		//cv::cvtColor(frame, frame, CV_BGR2RGB);
		
		itm.Process(frame, fcolor);
	
		std::cout << "Before Writing Result" << std::endl;
		cv::imwrite(outName, fcolor);
		std::cout << "After Writing Result" << std::endl << std::endl;
	}
	
	return 0;
}

//int main()
//{
//	cv::Mat res(1024,512, CV_32F);
//	PolygonsToImage pti;
//	PolygonsToImage::PolygonList pl;
//	PolygonsToImage::Polygon p;
//	p.push_back(cv::Point2d(0,0));
//	p.push_back(cv::Point2d(1024,0));
//	p.push_back(cv::Point2d(1024,512));
//	p.push_back(cv::Point2d(0,512));
//	pl.push_back(p);
//	pti.Process(res, pl, res);
//}

