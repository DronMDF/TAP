
#include <iostream>
#include <stdexcept>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

#include "CommandLineParser.h"
#include "ClientHonest.h"
#include "ClientManager.h"
#include "Logger.h"

using namespace std;
using namespace boost;

const string CommandLineParser::help =
	"Экстремальный тест СД на нагрузку/устойчивость\n"
	"\nИспользование:\n"
	"\txap help\n"
	"\txap <опции> load\n"
	"\txap <опции> attack <тип атаки>\n"
	"\nОпции:\n"
	"\t-s <сервер[:порт]>\n"
	"\t-c <сертификат>\n"
	"\t-k <ключ>\n"
	"\t-n <количество>\n";

CommandLineParser::CommandLineParser(int argc, const char **argv)
	: m_options(), m_args()
{
	list<string> args(argv, argv + argc);
	parse_options(args);
}

bool CommandLineParser::optionIsPresent(const std::string &op) const
{
	return m_options.find(op) != m_options.end();
}

void CommandLineParser::parse_options(const list<string> &args)
{
	string op;
	
	BOOST_FOREACH(const string &arg, args) {
		if (!op.empty()) {
			m_options[op] = arg;
			op.clear();
			continue;
		}

		BOOST_ASSERT(op.empty());
		if (arg[0] != '-') {
			m_args.push_back(arg);
			continue;
		}
		
		switch (arg[1]) {
			case 's': op = "server"; break;
			case 'c': op = "certfile"; break;
			case 'k': op = "keyfile"; break;
			case 'n': op = "num"; break;
			default: throw runtime_error("Неправильная опция");
		}
	}

	if (!op.empty()) {
		throw runtime_error("Неправильная опция");
	}

	if (optionIsPresent("server")) {
		// Постобработка сервера
		if (m_options["server"].find(':') == string::npos) {
			m_options["server"] += ":4433";
		}
	}
}


int CommandLineParser::run(ostream &out)
{
	if (m_args.front() == "help") {
		out << help;
		return 0;
	}

	if (m_args.front() == "load") {
		manageClients(bind(&CommandLineParser::createClientHonest, this));
		return 0;
	}
	
	return -1;
}

Client *CommandLineParser::createClientHonest() const
{
	if (!optionIsPresent("server")) {
		throw runtime_error("Необходимо указать опцию --server");
	}
	
	return new ClientHonest;
}

ClientManager *CommandLineParser::createClientManager(client_creator_t creator, uint num) const
{
	return new ClientManager(creator, num);
}

void CommandLineParser::manageClients(client_creator_t creator) const
{
	options_map_t::const_iterator ni = m_options.find("num");
	uint num = (ni != m_options.end()) ? lexical_cast<uint>(ni->second) : 1;
	scoped_ptr<ClientManager> mgr(createClientManager(creator, num));
	Logger log;
	mgr->setLogger(&log);
	mgr->run();
}
