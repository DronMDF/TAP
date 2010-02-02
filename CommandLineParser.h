
#pragma once

#include <iosfwd>
#include <list>
#include <string>

class CommandLineParser {
private:
	std::list<std::string> m_args;
	
public:
	CommandLineParser(int argc, const char **argv);
	int run(std::ostream &out);

	const static std::string help;
};
