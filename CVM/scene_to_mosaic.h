#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <cv.h>
#include "video_to_mhi.h"
#include "image_to_mosaic.h"

namespace videoMosaic
{

class SceneToMosaic;
class SceneManager
{
 public:
   SceneManager();
   ~SceneManager();

   typedef std::vector<cv::Rect> Motion;

   void Reset();
   int NextID();
   void SetImage(int id, cv::Mat img);
   void SetMotion(int id, const Motion& motion);
   void SetTransformation(int id, cv::Mat trans);
   cv::Mat GetImage(int id);
   Motion& GetMotion(int id);
   cv::Mat GetTransformation(int id);

   friend class SceneToMosaic;
 private:
   struct ImageData
   {
      cv::Mat image;
      cv::Mat transformation;
      cv::Mat pixelToPanoramaTrans;
      Motion motion;
   };

   typedef std::vector<ImageData> ImageDataVector;
   ImageDataVector m_data;
};

class SceneToMosaic
{
 public:
   SceneToMosaic(const boost::property_tree::ptree& ini);
   bool ProcessNext(cv::Mat frame);
   cv::Mat GetMosaic();

 private:
   SceneManager m_manager;
   VideoToMHI m_motion;
   ImageToMosaic m_itm;
};
}
