
#pragma once

#include <iosfwd>
#include <list>
#include <string>

class CommandLineParser {
private:
	std::list<std::string> m_args;

	virtual int runLoad(const std::string &server,
		const std::string &cert, const std::string &key, int count);
	
public:
	CommandLineParser(int argc, const char **argv);
	virtual ~CommandLineParser() {};
	
	int run(std::ostream &out);

	const static std::string help;
};
