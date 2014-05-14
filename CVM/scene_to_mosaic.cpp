#include "scene_to_mosaic.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <highgui.h>

namespace videoMosaic
{
SceneToMosaic::SceneToMosaic(const boost::property_tree::ptree& ini) : m_motion(ini), m_itm(ini) {}

namespace
{
   void CalculateValidMask(cv::Mat trans, const cv::Size& sz1, const cv::Size& sz, cv::Mat& mask)
{
   cv::Mat invMask;
	cv::Mat zeros = cv::Mat_<unsigned char>::zeros(sz1); 
	cv::warpPerspective(zeros, invMask, trans, sz, cv::INTER_NEAREST, cv::BORDER_CONSTANT, 1);
   mask = 1 - invMask;
}
//#undef RGBT
   typedef cv::Vec3b RGBT;

   RGBT CalculateRGBMedian(const std::vector<RGBT>& vec)
   {
      if (vec.empty())
      {
         return RGBT();
      }

      RGBT res;

      int medianIdx = vec.size() / 2;
      std::vector<unsigned char> tempVec;
      
      std::transform(vec.cbegin(), vec.cend(), std::back_inserter(tempVec), [](RGBT rgb)
                     { return rgb[0]; });
      std::nth_element(tempVec.begin(), tempVec.begin() + medianIdx, tempVec.end());
      res[0] = *(tempVec.begin() + medianIdx);
      tempVec.clear();

      std::transform(vec.cbegin(), vec.cend(), std::back_inserter(tempVec), [](RGBT rgb)
                     { return rgb[1]; });
      std::nth_element(tempVec.begin(), tempVec.begin() + medianIdx, tempVec.end());
      res[1] = *(tempVec.begin() + medianIdx);
      tempVec.clear();

      std::transform(vec.cbegin(), vec.cend(), std::back_inserter(tempVec), [](RGBT rgb)
                     { return rgb[2]; });
      std::nth_element(tempVec.begin(), tempVec.begin() + medianIdx, tempVec.end());
      res[2] = *(tempVec.begin() + medianIdx);
      tempVec.clear();
      return res;
   }

   cv::Mat GetWarpedMotionMask(const SceneManager::Motion& motion, cv::Mat trans,
                               const cv::Size& sz1, const cv::Size& sz)
   {
      cv::Mat_<unsigned char> imageMotion = cv::Mat_<unsigned char>::zeros(sz1);
      for (auto& rect: motion)
      {
         cv::rectangle(imageMotion, rect.tl(), rect.br(), 1, CV_FILLED);
      }
      cv::Mat res;
      cv::warpPerspective(imageMotion, res, trans, sz, CV_INTER_NN, cv::BORDER_CONSTANT, 0);
      return res;
   }
}
cv::Mat SceneToMosaic::GetMosaic()
{
   /*
      Outline:
      Use the transformation of every frame to calculate the coordinates of the corners.
      Let the main roi to be the max/min of all previous points.
      Pre Multiply the pixelToPanoramaTrans with a transformation that corrects for the calculated roi.
      Calculate median image by warping each frame to the calculated roi.
   */

   float minX, maxX, minY, maxY;
   minX = minY = std::numeric_limits<float>::max();
   maxX = maxY = -std::numeric_limits<float>::max();
   cv::Point2f tl(0,0);
   cv::Point2f br(m_manager.GetImage(0).size());
   std::vector<cv::Point2f> points;
   points.push_back(tl);
   points.push_back(br);
   for (auto& imageData : m_manager.m_data)
   {
      std::vector<cv::Point2f> transformed;
      cv::Mat floatTrans, invTrans;
      imageData.pixelToPanoramaTrans.convertTo(floatTrans, CV_32F);
      cv::invert(floatTrans, invTrans);
      cv::perspectiveTransform(points, transformed, invTrans);
      for (auto pt : transformed)
      {
         minX = std::min(minX, pt.x);
         maxX = std::max(maxX, pt.x);
         minY = std::min(minY, pt.y);
         maxY = std::max(maxY, pt.y);
      }
   }

   cv::Mat_<double> originTrans = cv::Mat_<double>::eye(3, 3);
   originTrans(0, 2) = minX;
   originTrans(1, 2) = minY;
   for (auto& imageData : m_manager.m_data)
   {
      imageData.pixelToPanoramaTrans = originTrans * imageData.pixelToPanoramaTrans;
   }

   std::cout << minX << " : " << maxX << " : " << minY << " : " << maxY <<std::endl;
   cv::Size sz(std::ceil(maxX - minX), std::ceil(maxY - minY));
   
   cv::Mat_<RGBT> res;
   cv::Mat_<RGBT> medianImage = cv::Mat_<RGBT>::zeros(sz);
   std::vector<std::vector<RGBT>> pixelsVector(sz.area());
   int idx = 0;
   for (auto& imageData : m_manager.m_data)
   {
      cv::Mat invTrans;
      cv::invert(imageData.pixelToPanoramaTrans, invTrans);
      cv::Mat singleWarp;
      cv::Mat warpMask;
      cv::warpPerspective(imageData.image, singleWarp, invTrans, sz, CV_INTER_CUBIC, cv::BORDER_REFLECT);
      cv::Size frameSz(imageData.image.cols, imageData.image.rows);
      CalculateValidMask(invTrans, frameSz, sz, warpMask);
      cv::Mat motionMask = GetWarpedMotionMask(imageData.motion, invTrans, frameSz, sz);
      for (int row = 0; row < sz.height; ++row)
      {
         for (int col = 0; col < sz.width; ++col)
         {
            if (warpMask.at<unsigned char>(cv::Point(col, row)) == 1 &&
                motionMask.at<unsigned char>(cv::Point(col, row)) == 0)
            {
               int idx = row * sz.width + col;
               pixelsVector[idx].push_back(singleWarp.at<RGBT>(cv::Point(col, row)));
            }
         }
      }
      std::cout << "WARP " << ++idx << std::endl;
   }

   idx = 0;
   for (int row = 0; row < sz.height; ++row)
      {
         for (int col = 0; col < sz.width; ++col)
         {
            cv::Point pt(col, row);
            int idx = row * sz.width + col;
            auto& vec = pixelsVector[idx];
            RGBT val = CalculateRGBMedian(vec);
            medianImage(pt) = val;
         }
         std::cout << "MEDIAN ROW " << ++idx << std::endl;
      }
   cv::imwrite("pano_orig.png", medianImage);
   m_itm.Process(medianImage, res);
   return res;

}

bool SceneToMosaic::ProcessNext(cv::Mat frame)
{
   int id = m_manager.NextID();
   m_manager.SetImage(id, frame);
   cv::Mat gray;
   cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
   m_motion.Give(gray, true); // Maybe convert to gray?
   cv::Mat trans;
   bool res = m_motion.TakeGlobalTrans(trans);
   assert(id == 0 || res);
   m_manager.SetTransformation(id, trans);
   SceneManager::Motion motion;
   m_motion.TakeSegmentation(motion);
   m_manager.SetMotion(id, motion);
   return true;
}

SceneManager::SceneManager() {}
SceneManager::~SceneManager() { Reset(); }

void SceneManager::Reset() {}

int SceneManager::NextID()
{
   m_data.push_back(ImageData());
   return static_cast<int>(m_data.size() - 1);
}

void SceneManager::SetImage(int id, cv::Mat img) { m_data[id].image = img; }
void SceneManager::SetMotion(int id, const std::vector<cv::Rect>& motion)
{
   m_data[id].motion = motion;
}

void SceneManager::SetTransformation(int id, cv::Mat trans)
{
   if (id == 0)
   {
      m_data[id].pixelToPanoramaTrans = cv::Mat_<double>(3, 3);
      cv::setIdentity(m_data[id].pixelToPanoramaTrans);
      m_data[id].transformation = m_data[id].pixelToPanoramaTrans.clone();
   }
   else
   {
      m_data[id].transformation = trans;
      m_data[id].pixelToPanoramaTrans = m_data[id - 1].pixelToPanoramaTrans * trans;
   }
}
cv::Mat SceneManager::GetImage(int id) { return m_data[id].image; }
SceneManager::Motion& SceneManager::GetMotion(int id) { return m_data[id].motion; }
cv::Mat SceneManager::GetTransformation(int id) { return m_data[id].transformation; }
}