
#include "CommandLineParser.h"

using namespace std;

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
	m_args.erase(m_args.begin());
}

int CommandLineParser::run(ostream &out)
{
	if (m_args.front() == "help") {
		out << help;
		return 0;
	}
	
	return 0;
}
