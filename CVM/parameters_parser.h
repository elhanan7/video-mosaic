#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>

namespace videoMosaic
{
	void ParseParameters(int argc, char** argv, boost::property_tree::ptree& p, std::vector<std::string>& unparsed);
}
