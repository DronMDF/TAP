
#pragma once

#include <iosfwd>
#include <string>
#include <list>
#include <map>
#include <boost/function.hpp>

class Client;
class ClientManager;

class CommandLineParser {
private:
	void parse_options(const std::list<std::string> &args);

	Client *createHonestClient() const;
	
protected:
	typedef std::map<std::string, std::string> options_map_t;
	typedef boost::function<Client *()> client_creator_t;
	
	options_map_t m_options;
	std::list<std::string> m_args;

	virtual ClientManager *createClientManager(client_creator_t creator, uint num) const;
	virtual void manageClients(client_creator_t creator) const;
	
public:
	CommandLineParser(int argc, const char **argv);
	virtual ~CommandLineParser() {};
	
	int run(std::ostream &out);

	const static std::string help;
};
