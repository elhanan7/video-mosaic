#pragma once
#include "video_source.h"

#include <set>
class DirectorySource :
	public VideoSource
{
public:
	DirectorySource(std::string path, std::string pattern, int startIndex, int stopIndex);

	virtual cv::Mat Next();
	virtual bool HasNext() const;
	virtual void    Reset();
	virtual cv::Size Size() const;
	virtual int  Type() const;

	std::string GetName() const;

private:
	typedef std::set<std::string> PathVector;
	
	PathVector m_paths;
	PathVector::const_iterator m_current;
	int m_type;
	int m_rows, m_cols;

};

