
#pragma once

#include <iosfwd>
#include <string>
#include <list>
#include <map>

class CommandLineParser {
private:
	virtual int callUp(int count);
	void parse_options(const std::list<std::string> &args);

protected:
	std::map<std::string, std::string> m_options;
	std::list<std::string> m_args;
	
public:
	CommandLineParser(int argc, const char **argv);
	virtual ~CommandLineParser() {};
	
	int run(std::ostream &out);

	const static std::string help;
};
