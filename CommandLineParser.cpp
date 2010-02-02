
#include <iostream>
#include <stdexcept>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>

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
	: m_args(argv, argv + argc)
{
	m_args.pop_front();
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
			return runLoad(server, cert, key, num);
		}
	}
	
	return -1;
}

int CommandLineParser::runLoad(const string &server,
		const string &cert, const string &key, int count)
{
	return 0;
}
