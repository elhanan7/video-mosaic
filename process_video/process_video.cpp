// process_video.cpp : Defines the entry point for the console application.
//

#include <cv.h>
#include <highgui.h>

#include <video_to_mosaic.h>

#include <boost/property_tree/ini_parser.hpp>


int main(int argc, char* argv[])
{
	std::string inName(argv[1]);
	std::string outName(argv[2]);
	cv::VideoCapture vreader(inName);
	cv::Ptr<cv::VideoWriter> vwriter;
	cv::Mat inMat, rgbMat, outMat, outBGR;
	boost::property_tree::ptree ini;
	boost::property_tree::ini_parser::read_ini("cvm.ini", ini);
	videoMosaic::VideoToMosaic vtm(ini);
	int idx = 0;
	while (vreader.read(inMat))
	{
		cv::cvtColor(inMat, rgbMat, cv::COLOR_BGR2RGB);
		vtm.ProcessNext(inMat, outMat);
		cv::cvtColor(outMat, outBGR, cv::COLOR_RGB2BGR);
		if (vwriter.empty())
		{
			vwriter = new cv::VideoWriter(outName, CV_FOURCC('X','V','I','D'), vreader.get(CV_CAP_PROP_FPS), outBGR.size()); 
		}
		(*vwriter) << outMat;
		std::cout << "Frame " << (idx++) << " done." << std::endl;
	}
	return 0;
}

