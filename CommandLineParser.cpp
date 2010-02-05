
#include <iostream>
#include <stdexcept>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "CommandLineParser.h"

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
	args.pop_front();
	parse_options(args);
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

	if (m_options.find("server") != m_options.end()) {
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
		//int num = (m_options.find("num") != m_options.end()) ?
		//		lexical_cast<int>(m_options["num"]) : 1;
		//callUp(num);
		return 0;
	}
	
	return -1;
}

void CommandLineParser::callUp(creator_callback_t creator)
{
}
