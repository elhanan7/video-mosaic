#include <highgui.h>
#undef BOOST_REGEX_NO_LIB
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include "directory_source.h"

namespace bfs = boost::filesystem;

namespace videoMosaic {

DirectorySource::DirectorySource(const std::string& path, const std::string& pattern, int startIndex, int stopIndex, bool reverse)
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
	m_currentr = m_paths.rbegin();
	cv::Mat first = cv::imread(*m_current);
	m_type = first.type();
	m_rows = first.rows;
	m_cols = first.cols;
	m_reverse = reverse;
}

cv::Mat DirectorySource::Next()
{
	if (!m_reverse)
	{
		return cv::imread(*(m_current++));
	}
	else
	{
		return cv::imread(*(m_currentr++));
	}
}

bool DirectorySource::HasNext() const
{
	if (!m_reverse)
	{
		return (m_current != m_paths.end());
	}
	else
	{
		return (m_currentr != m_paths.rend());
	}
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
	m_currentr = m_paths.rbegin();
}

std::string DirectorySource::GetName() const
{
	if (!m_reverse)
	{
		PathVector::const_iterator cit = m_current;
		return *(--cit);
	}
	else
	{
		PathVector::const_reverse_iterator cit = m_currentr;
		return *(--cit);
	}
}

}


