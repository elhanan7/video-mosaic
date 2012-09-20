#include <cv.h>
#include <highgui.h>
#include <osgViewer/Viewer>
#include <osg/Node>

#include "image_to_mosaic.h"
#include "directory_source.h"
#include "video_to_mhi.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

using namespace cv;

const int TSIZE(4);

int main(int argc, char** argv)
{
	Mat_<cv::Vec3b> frame, fcolor;
	Mat edges;

	cv::Mat_<unsigned char> final;
	DirectorySource ds(".", "frame(\\d+)\\.png",0,0);

	boost::property_tree::ptree ini;
	boost::property_tree::ini_parser::read_ini("C:\\Users\\elhanan7\\Programming\\CVM\\Data\\INI\\cvm.ini", ini);
	ImageToMosaic itm(ini);
	VideoToMHI vtm(ini);
	bool first = true;
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
		
		//itm.Process(frame, fcolor);
		cv::Mat gray;
		cv::cvtColor(frame, gray, CV_BGR2GRAY);
		vtm.Give(gray);
		cv::Mat_<float> mhi = vtm.Take().clone();
		std::vector<cv::Rect> segs;
		vtm.TakeSegmentation(segs);
		std::cout << segs.size() << std::endl;
		cv::Mat_<unsigned char> motionMask = cv::Mat::zeros(frame.rows, frame.cols, CV_8U);
		for (auto iter = segs.cbegin(); iter != segs.cend(); ++iter)
		{
			cv::Rect rect = *iter;
			cv::Size axes(1.5*rect.width, 1.5*rect.height);
			cv::Point center(rect.x + 0.5*rect.width, rect.y + 0.5*rect.height);
			cv::ellipse(motionMask, center, axes,0 ,0, 360,cv::Scalar(1),-1);
		}
		if (!first)
		{
			itm.Process(frame, fcolor, motionMask);
		}
		else
		{
			itm.Process(frame, fcolor);
			first = false;
		}
		cv::imwrite(topoName, mhi);
		
		//std::cout << "Before Writing Result" << std::endl;
		cv::imwrite(outName, fcolor);
		//std::cout << "After Writing Result" << std::endl << std::endl;
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

