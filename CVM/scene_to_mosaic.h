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
   void SetMHI(int id, cv::Mat trans);
   void SetTimeStamp(int id, double ts);
   cv::Mat GetImage(int id);
   Motion& GetMotion(int id);
   cv::Mat GetTransformation(int id);

   friend class SceneToMosaic;
 private:
   typedef std::vector<cv::Rect> Motion;
   struct ImageData
   {
      cv::Mat image;
      cv::Mat transformation;
      cv::Mat pixelToPanoramaTrans;
      Motion motion;
      cv::Mat mhi;
      double timeStamp;
   };

   typedef std::vector<ImageData> ImageDataVector;
   ImageDataVector m_data;
};

class SceneToMosaic
{
 public:
   SceneToMosaic(const boost::property_tree::ptree& ini);
   bool ProcessNext(cv::Mat frame);
   void ProcessAll();
   bool HasNext();
   cv::Mat RecieveNext();

 private:
   cv::Mat CreateMotionMask(SceneManager::Motion& motion, cv::Mat mhi, double timeStamp,
                            cv::Size dstSize);
   SceneManager m_manager;
   VideoToMHI m_motion;
   ImageToMosaic m_itm;
   cv::Mat m_pano;
   cv::Mat m_panoGuideLines;
   cv::Mat m_panoTileMask;
   PolygonList m_panoPolygons;
   int m_currentIdx;
   double m_motionExpansionFactor;
   bool m_followMotionStrictly;
   bool m_calculateGlobalMotion;
};
}
