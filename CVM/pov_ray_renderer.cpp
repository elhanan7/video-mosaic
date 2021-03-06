#include "pov_ray_renderer.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <highgui.h>
#include <boost/property_tree/ptree.hpp>

namespace bpt = boost::property_tree;

namespace videoMosaic {

namespace
{
	double maxX(0), maxY(0);
	std::string GenerateTempFileName()
	{
		char buffer [L_tmpnam];
		memset(buffer, 0, L_tmpnam * sizeof(char));
		tmpnam(buffer);
		std::string tmpNameWithDots(buffer), tmpName;
		std::copy_if(tmpNameWithDots.begin(), tmpNameWithDots.end(),
			std::back_inserter(tmpName), [](char c) -> bool {return c != '.' &&
																	c != '\\' &&
																	c != '/';});
		return tmpName;
	}

	std::string SerializeVertex(cv::Point2d vert)
	{
		if (vert.x > maxX) maxX = vert.x;
		if (vert.y > maxY) maxY = vert.y;
		std::stringstream sts;
		sts << "<" << std::setprecision(32) << vert.x << ","<< std::setprecision(32) <<vert.y << ">";
		return sts.str();
	}

	std::string SerializePolygon(cv::Size2f sz, 
		                         const std::vector<cv::Point2d>& p, 
								 cv::Vec3b color,
								 float height = 2.0f)
	{
		std::stringstream sts;
		auto vertex_fixer = [sz](cv::Point2d pt) -> cv::Point2d
		{
			return cv::Point2d(pt.x, sz.height - pt.y);
		};
		sts << "prism { 0, " << height << "," << (p.size() + 1) << ", ";
		std::for_each(p.cbegin(), p.cend(), [&sts,vertex_fixer](cv::Point2d pt)
		{
			sts << SerializeVertex(vertex_fixer(pt)) << ",";
		});
		sts << SerializeVertex(vertex_fixer(*p.begin())) << " ";
		sts << "texture {pigment { color rgb <" << 
			color(2)/255.0f << "," <<
			color(1)/255.0f << "," <<
			color(0)/255.0f << "," <<
			"> }} texture {T_Grnt22a }  }\n";
		return sts.str();
	}
}

PovRayRenderer::PovRayRenderer(const bpt::ptree& ini)
{
	m_sizeMultiplier = ini.get("PovRayRenderer.SizeMultiplier", 2);
	m_povrayPath = ini.get("PovRayRenderer.PovRayPath", "povray");
	m_povIniPath = ini.get("PovRayRenderer.PovRayIniPath", "povray.ini");
	m_tileHeight = ini.get("PovRayRenderer.TileHeight", 2.0f);
	m_antiAlias  = ini.get("PovRayRenderer.Antialiasing", true);

}


void PovRayRenderer::Process(const PolygonList& polygons, cv::Size sz, cv::Mat& res)
{
	cv::Size2f halfSize(sz.width/2.0f, sz.height/2.0f);

	std::string tempNameNoExt = GenerateTempFileName();
	std::string tempName = tempNameNoExt + ".pov";
	std::string tempResName = tempNameNoExt + ".png";
	std::ofstream file(tempName);
	file << "#version 3.6\n#include \"colors.inc\"\n#include \"stones.inc\"\nlight_source  {\n<";
	file << halfSize.width << "," << halfSize.width / 2 << "," << 0.85*halfSize.height << "> White parallel point_at<" << halfSize.width << ",0," << 0.85*halfSize.height << "> }\n";
	file << "camera {\nlocation <";
    file << halfSize.width <<","<<halfSize.width<<","<<halfSize.height<<">\nup <0,1,0>\n";
	file << "right <" << (halfSize.width / halfSize.height) << ",0,0>\n";
	file << "look_at <" << 
	halfSize.width <<","<< 0 <<","<<halfSize.height
	<< ">\n";
	file << "angle " << std::atan(halfSize.width)*(180/M_PI) << "\n}\n";
	cv::Vec3b backColor(190,190,190);
	file << "plane { y, -0.02 pigment { color rgb <" << 
	(backColor(2)/255.0f) <<","<<(backColor(1)/255.0f)<<","<<(backColor(0)/255.0f) 
	<< ">} }\n";

	for (auto iter = polygons.cbegin(); iter != polygons.cend(); ++iter)
	{
		file << SerializePolygon(sz, iter->vertices, iter->color, m_tileHeight);
	}
	file.close();
	std::string aaStr = m_antiAlias? " +A " : " ";
	std::stringstream cmdStream;
	cmdStream << m_povrayPath << aaStr << " -D " << " ";
	cmdStream << " +W" << sz.width*m_sizeMultiplier << " +H" << sz.height*m_sizeMultiplier << " " << tempName << " > nul";
	system(cmdStream.str().c_str());
	res = cv::imread(tempResName);
	remove(tempName.c_str());
	remove(tempResName.c_str());
	
}

}
