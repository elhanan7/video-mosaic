#include "parameters_parser.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/adapted.hpp>

namespace qi = boost::spirit::qi;
namespace fusion = boost::fusion;
using qi::ascii::space_type;
namespace phx = boost::phoenix;

namespace videoMosaic
{
	namespace detail
	{
		struct key_value_t
		{
			std::string section;
			std::string key;
			std::string value;
		};

		struct ini_file_t
		{
			std::string baseName;
		};

		struct ini_write_t
		{
			ini_file_t ini;
		};

		typedef std::vector<boost::variant<key_value_t, ini_write_t, std::string> > ast_t;

	}
}

BOOST_FUSION_ADAPT_STRUCT(
	videoMosaic::detail::key_value_t,
	(std::string, section)
	(std::string, key)
	(std::string, value) );

BOOST_FUSION_ADAPT_STRUCT(
	videoMosaic::detail::ini_file_t,
	(std::string, baseName)	);

BOOST_FUSION_ADAPT_STRUCT(
	videoMosaic::detail::ini_write_t,
	(videoMosaic::detail::ini_file_t, ini) );

namespace videoMosaic { namespace detail {
	template <typename iter_t>
	class parameter_parser : public qi::grammar<iter_t, ast_t(), space_type>
	{
	public:
		parameter_parser() : parameter_parser::base_type(start)
		{
			key_value_pair %= qi::lexeme[(+qi::alnum) >> '.' >> (+qi::alnum) >> '=' >> (+qi::alnum)];
			ini_file %= qi::lexeme[+(qi::char_ - ';')] >> ';';
			write_ini %= "write:" >> ini_file;

			start %= *(key_value_pair | write_ini | ini_file );
		}

		qi::rule<iter_t, key_value_t() ,space_type> key_value_pair; 
		qi::rule<iter_t, std::string() ,space_type> ini_file;
		qi::rule<iter_t, ini_write_t() ,space_type> write_ini;
		qi::rule<iter_t, ast_t() ,space_type> start;
	};

	struct apply_actions : boost::static_visitor<>
	{
		apply_actions(boost::property_tree::ptree& p) : m_ptree(p) {};
		
		void operator()(const std::string& v)
		{
			boost::property_tree::ini_parser::read_ini(v, m_ptree);
		}

		void operator()(const ini_write_t& v)
		{
			boost::property_tree::ini_parser::write_ini(v.ini.baseName, m_ptree);
		}

		void operator()(const key_value_t& v)
		{
			m_ptree.put(v.section + "." + v.key, v.value);
		}

		boost::property_tree::ptree& m_ptree;
	};
} 

bool parse_parameters(int argc, char** argv, boost::property_tree::ptree& p)
{
	std::stringstream sts;
	for (int i = 0; i < argc; ++i)
	{
		sts << argv[i] << " ";
	}
	detail::ast_t ast;
	std::string str(sts.str());
	std::string::const_iterator it = str.begin();
	bool res = phrase_parse(it, str.cend(), detail::parameter_parser<std::string::const_iterator>(), space_type(), ast);
	if (!res || it != str.cend()) return false;
	detail::apply_actions aa(p);
	for (auto iter = ast.cbegin(); iter != ast.cend(); ++iter)
	{
		iter->apply_visitor(aa);
	}
}

}
