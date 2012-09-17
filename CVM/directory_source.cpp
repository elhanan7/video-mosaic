#include <highgui.h>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include "directory_source.h"

namespace bfs = boost::filesystem;


DirectorySource::DirectorySource(std::string path, std::string pattern, int startIndex, int stopIndex)
{
	boost::regex re(pattern);
	bfs::directory_iterator endIter;
	for (bfs::directory_iterator itr(path); itr != endIter; ++itr)
	{
		if (regex_match(itr->path().filename().string(), re))
		{
			m_paths.insert(itr->path().string());
		}
	}
	m_current = m_paths.begin();
	cv::Mat first = cv::imread(*m_current);
	m_type = first.type();
	m_rows = first.rows;
	m_cols = first.cols;
}

cv::Mat DirectorySource::Next()
{
	return cv::imread(*(m_current++));
}

bool DirectorySource::HasNext() const
{
	return (m_current != m_paths.end());
}

int DirectorySource::Type() const
{
	return m_type;
}

cv::Size DirectorySource::Size() const
{
	return cv::Size(m_cols, m_rows);
}

void DirectorySource::Reset()
{
	m_current = m_paths.begin();
}

std::string DirectorySource::GetName() const
{
	PathVector::const_iterator cit = m_current;
	return *(--cit);
}


