
#pragma once

#include <iosfwd>
#include <string>
#include <list>
#include <map>
#include <boost/function.hpp>

class Client;

class CommandLineParser {
private:
	void parse_options(const std::list<std::string> &args);

protected:
	typedef std::map<std::string, std::string> options_map_t;
	typedef boost::function<Client *(const options_map_t &)> creator_callback_t;
	
	options_map_t m_options;
	std::list<std::string> m_args;
	
	virtual void callUp(creator_callback_t callback);
	
public:
	CommandLineParser(int argc, const char **argv);
	virtual ~CommandLineParser() {};
	
	int run(std::ostream &out);

	const static std::string help;
};
