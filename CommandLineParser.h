
#pragma once

#include <iosfwd>
#include <string>
#include <list>
#include <map>
#include <boost/function.hpp>

#include "Client.h"

class ClientManager;

class CommandLineParser {
private:
	void parse_options(const std::list<std::string> &args);

	bool optionIsPresent(const std::string &op) const;
	
protected:
	typedef std::map<std::string, std::string> options_map_t;
	
	options_map_t m_options;
	std::list<std::string> m_args;

	virtual ClientManager *createClientManager(client_creator_t creator, uint num) const;
	virtual void manageClients(client_creator_t creator) const;
	
	Client *createClientHonest() const;

public:
	CommandLineParser(int argc, const char **argv);
	virtual ~CommandLineParser() {};
	
	int run(std::ostream &out);

	const static std::string help;
};
