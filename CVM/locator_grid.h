#pragma once

#include <cv.h>
#include <boost/multi_array.hpp>
#include <vector>

namespace videoMosaic {

class LocatorGrid
{
public:

	LocatorGrid(cv::Size sz, int tsize) : m_grid(boost::extents[(sz.height + tsize - 1) / tsize][(sz.width  + tsize - 1) / tsize]), 
		m_tsize(tsize)
	{}

	void Add(const cv::Point& location, size_t idx)
	{
		int h = location.y / m_tsize;
		int w = location.x / m_tsize;
		m_grid[h][w].push_back(idx);
	}

	void GetHood(const cv::Point& location, std::vector<size_t>& hood)
	{
		int h = location.y / m_tsize;
		int w = location.x / m_tsize;
		int minH = std::max(h - 1, 0);
		int maxH = std::min(h + 1, static_cast<int>(m_grid.shape()[0]));
		int minW = std::max(w - 1, 0);
		int maxW = std::min(w + 1, static_cast<int>(m_grid.shape()[1]));
		hood.clear();
		for (int i = minH; i < maxH; ++i)
		{
			for (int j = minW; j < maxW; ++j)
			{
				hood.insert(hood.end(), m_grid[i][j].begin(), m_grid[i][j].end());
			}
		}
	}

private:

	typedef	boost::multi_array<std::vector<size_t>, 2> Grid;
	Grid m_grid;
	int m_tsize;
};

}
