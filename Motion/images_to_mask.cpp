#include <cv.h>
#include "images_to_mask.h"


ImagesToMask::ImagesToMask(void)
{
}


ImagesToMask::~ImagesToMask(void)
{
}

void ImagesToMask::Process(const cv::Mat& a, const cv::Mat b, cv::Point2f globalTrans, cv::Mat& mask)
{
	cv::Mat1f flow(a.rows, a.cols);
	cv::Mat1f sdv(a.rows, a.cols);
	
	a.copyTo(sdv);
	cv::GaussianBlur(sdv, flow /*abuse of flow*/, cv::Size(23,23), 0);
	cv::absdiff(sdv,flow /*abuse*/,sdv);

	std::vector<cv::Mat1f> dxdy(2);
	cv::calcOpticalFlowFarneback(a,b,dxdy,0.5, 4, 9,20,7,1.1,0);
	dxdy[0] -= globalTrans.x;
	dxdy[1] -= globalTrans.y;
	cv::multiply(dxdy[0], dxdy[0], dxdy[0]);
	cv::multiply(dxdy[1], dxdy[1], dxdy[1]);
	cv::add(dxdy[0],dxdy[1], flow);
	cv::sqrt(flow, flow);
	cv::Mat mean, oneDeviationMat;
	cv::meanStdDev(flow, mean, oneDeviationMat);
	float oneDeviation = oneDeviationMat.at<float>(0);
	std::cout << oneDeviation << std::endl;
}
