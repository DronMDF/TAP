
#include <sstream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <boost/test/utils/nullstream.hpp>

#include "../CommandLineParser.h"
#include "testUtility.h"

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(suiteCommandLineParser);

static onullstream nullout;

BOOST_AUTO_TEST_CASE(testOptions)
{
	const string server = "test:1234";
	const string cert = "cert.test";
	const string key = "key.test";
	const string num = "8000";
	const char *argv[] = { "xap",
		"-s", server.c_str(),
		"-c", cert.c_str(),
		"-k", key.c_str(),
		"-n", num.c_str()
	};
	
	struct testCLP : public CommandLineParser {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		using CommandLineParser::m_options;
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);

	BOOST_REQUIRE_EQUAL(cmd.m_options["server"], server);
	BOOST_REQUIRE_EQUAL(cmd.m_options["certfile"], cert);
	BOOST_REQUIRE_EQUAL(cmd.m_options["keyfile"], key);
	BOOST_REQUIRE_EQUAL(cmd.m_options["num"], num);
}

BOOST_AUTO_TEST_CASE(testServerComplete)
{
	const string server = "test";
	const char *argv[] = { "xap", "-s", server.c_str() };

	struct testCLP : public CommandLineParser {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		using CommandLineParser::m_options;
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);

	BOOST_REQUIRE_EQUAL(cmd.m_options["server"], server + ":4433");
}

BOOST_AUTO_TEST_CASE(testCommands)
{
	const string command = "load";
	const char *argv[] = { "xap", "-n", "666", command.c_str() };

	struct testCLP : public CommandLineParser, private visit_mock {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		void callUp(int num) { visit(); BOOST_REQUIRE_EQUAL(num, 666); }
		using CommandLineParser::m_args;
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);
	cmd.run(nullout);
	
	BOOST_REQUIRE_EQUAL(cmd.m_args.front(), command);
}

BOOST_AUTO_TEST_CASE(testHelp)
{
	const char *argv[] = { "xap", "help", };
	CommandLineParser cmd(2, argv);

	ostringstream out;
	cmd.run(out);

	BOOST_REQUIRE_EQUAL(out.str(), CommandLineParser::help);
}

BOOST_AUTO_TEST_SUITE_END();
