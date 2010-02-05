
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

	string server = "localhost:4433";
	string cert = "user.cer";
	string key = "user.key";
	int num = 1;

	while (!m_args.empty()) {
		const string op = m_args.front();
		m_args.pop_front();
		
		if (op[0] == '-') {
			BOOST_ASSERT(!m_args.empty());
			
			switch (op[1]) {
				case 's':
					server = m_args.front();
					if (server.find(':') == string::npos) {
						server += ":4433";
					}
					break;
					
				case 'c':
					cert = m_args.front();
					break;

				case 'k':
					key = m_args.front();
					break;

				case 'n':
					num = lexical_cast<uint32_t>(m_args.front());
					break;
				default:
					cerr << "Неправильная опция: " << op << endl;
					throw runtime_error("Неправильная опция");
			}

			m_args.pop_front();
			continue;
		}
			
		if (op == "load") {
			return callUp(num);
		}
	}
	
	return -1;
}

int CommandLineParser::callUp(int count)
{
	return 0;
}
