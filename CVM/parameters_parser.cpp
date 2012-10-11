#include "parameters_parser.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/regex.hpp>

namespace
{
	boost::regex key_value_re("(.*)\\.(.*)=(.*)");
	boost::regex ini_file_re("(.*\\.ini)");
	boost::regex write_ini_re("write:(.*\\.ini)");

	bool ParseOne(const char* one, boost::property_tree::ptree& pt)
	{
		boost::cmatch mr;
		if (regex_match(one, mr, key_value_re))
		{
			pt.put(mr[1] + "." + mr[2], mr[3]);
		}
		else if (regex_match(one, mr, write_ini_re))
		{
			boost::property_tree::ini_parser::write_ini(mr[1], pt);
		}
		else if (regex_match(one, mr, ini_file_re))
		{
			boost::property_tree::ini_parser::read_ini(mr[1], pt);
		}
		else
		{
			return false;
		}
		return true;
	}
} 

namespace videoMosaic 
{
bool parse_parameters(int argc, char** argv, boost::property_tree::ptree& p)
{
	for (int i = 0; i < argc; ++i)
	{
		if (!ParseOne(argv[i], p)) return false;
	}
	return true;
}

}
